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
static int parse_objects(void)
{
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_list metadata_list = { 0 };
	void *stream = NULL;

	status = oem_prov_load_assets(&stream);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Error loading assets file\n");
		return status;
	}
	status = oem_prov_extract_blobs_metadata(stream, &metadata_list);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Error parsing assets file\n");
		goto exit;
	}
	status = oem_prov_import_blob_by_id(stream, metadata_list, SRKH_KEY_ID);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;
	status = oem_prov_import_all_blobs(stream, metadata_list);
	oem_prov_list_destroy(&metadata_list);

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	oem_prov_unload_assets(stream);
	return status;
}

int oem_prov_indirect(const char *config_filename)
{
	int status = OEM_PROV_STATUS_OK;
	int close_option = 0;
	psa_status_t psa_status = PSA_SUCCESS;

	status = oem_prov_load_config(config_filename, OEM_PROV_INDIRECT);
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

	status = parse_objects();
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	/* check if the key storage should be committed */
	if (oem_prov_get_storage(OEM_PROV_INDIRECT)) {
		status = oem_prov_commit_key_storage();
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	/* check if the device needs to be closed */
	close_option = oem_prov_get_lc_option(OEM_PROV_INDIRECT);
	if (close_option)
		status = oem_prov_set_lifecycle(close_option);

exit:
	oem_prov_unload_config();
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}
