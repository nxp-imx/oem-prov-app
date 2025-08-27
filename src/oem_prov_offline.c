// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2023-2025 NXP
 */
#include "oem_prov.h"
#include "oem_prov_blob.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_common.h"

/**
 * parse_objects() - Parses the stream containing the security assets,
 *			extracts them and imports them into ELE
 *
 * If there is an asset containing SRKH, it is imported first. SRKH is used by
 * ELE to derive keys that are further used for provisioning the other assets.
 * Therefore is mandatory to be imported first.
 *
 * Return:
 * error code
 */
static int parse_objects(char *path)
{
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_list metadata_list = { 0 };
	void *stream = NULL;
	int found = 0;
	int prov_type = 0;

	status = oem_prov_load_assets(0, path, &stream);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Error loading assets file\n");
		return status;
	}
	status = oem_prov_extract_blobs_metadata(stream, &metadata_list);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Error parsing assets file\n");
		goto exit;
	}

	prov_type = oem_prov_get_prov_type();

	/* If the user did not specify the flow type in the configuration file
	 * the application searches for the OEM key.
	 * If the user specified the flow type in the configuration file
	 * skip the search for the OEM key if product based provisioning is
	 * not selected
	 */
	if (!prov_type || prov_type == OEM_PROV_FLOW_PROD) {
		status = oem_prov_import_blob_by_id(stream, metadata_list,
						    EL2GO_PROV_OEM_KEY_ID, &found);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;

		if (!found && prov_type == OEM_PROV_FLOW_PROD) {
			status = OEM_PROV_STATUS_INVALID_BLOB;
			OEM_PROV_DBG_PRINTF(ERROR, "The OEM key is missing\n");
			goto exit;
		}
	}

	status = oem_prov_import_blob_by_id(stream, metadata_list, SRKH_KEY_ID,
					    &found);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;
	status = oem_prov_import_all_blobs(stream, metadata_list);
	oem_prov_list_destroy(&metadata_list);

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	oem_prov_unload_assets(stream);
	return status;
}

int oem_prov_offline(const char *config_filename)
{
	int status = OEM_PROV_STATUS_OK;
	int close_option = 0;
	psa_status_t psa_status = PSA_SUCCESS;
	char *path = NULL;

	status = oem_prov_load_config(config_filename, OEM_PROV_OFFLINE);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid configuration file!\n");
		return status;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "PSA crypto initialization\n");
	psa_status = psa_crypto_init();
	if (psa_status != PSA_SUCCESS) {
		status = OEM_PROV_STATUS_PSA_ERROR;
		goto exit;
	}
	status = oem_prov_load_env(&path);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	status = parse_objects(path);
	if (status != OEM_PROV_STATUS_OK) {
		oem_prov_unload_env(&path);
		goto exit;
	}

	status = oem_prov_unload_env(&path);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	/* check if the key storage should be committed */
	if (oem_prov_get_storage(OEM_PROV_OFFLINE)) {
		status = oem_prov_commit_key_storage();
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	/* check if the device needs to be closed */
	close_option = oem_prov_get_lc_option(OEM_PROV_OFFLINE);
	if (close_option)
		status = oem_prov_set_lifecycle(close_option);

exit:
	oem_prov_unload_config();
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}
