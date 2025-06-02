// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025 NXP
 */
#include <smw_device.h>

#include "oem_prov_blob.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "psa/crypto.h"
#include "psa/internal_trusted_storage.h"

static int oem_prov_get_lifecycle(smw_lifecycle_t *lifecycle)
{
	int status = OEM_PROV_STATUS_OK;
	struct smw_device_lifecycle_args smw_args = { 0 };
	int res = SMW_STATUS_OK;

	smw_args.subsystem_name = SMW_SUBSYSTEM_NAME_ELE;

	res = smw_device_get_lifecycle(&smw_args);
	if (res != SMW_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Get device lifecycle failed: %d\n",
				    res);
		status = OEM_PROV_STATUS_SMW_ERROR;
	}
	*lifecycle = smw_args.lifecycle_name;
	return status;
}

int oem_prov_import_blob(unsigned char *data, psa_key_attributes_t *attributes,
			 size_t length)
{
	psa_status_t status = PSA_SUCCESS;
	psa_key_id_t blob_id = PSA_KEY_ID_NULL;
	psa_key_id_t ret_blob_id = PSA_KEY_ID_NULL;

	blob_id = psa_get_key_id(attributes);
	OEM_PROV_DBG_PRINTF(VERBOSE, "Importing object (id: 0x%08x)\n",
			    blob_id);

	/* If SRKH is not already imported, the delete function will return an
	 * error, because it cannot delete an asset that does not exist. If the
	 * SRKH is already imported, the delete function will return an error,
	 * because, once imported, the SRKH cannot be deleted. So, it makes no
	 * sense to try to erase it.
	 * The EL2GO_PROV_OEM_KEY is not persistent, so there is no reason for
	 * deleting it.
	 */
	if (blob_id != SRKH_KEY_ID && blob_id != EL2GO_PROV_OEM_KEY_ID) {
		status = psa_destroy_key(blob_id);
		OEM_PROV_DBG_PRINTF(DEBUG,
				    "psa_destroy_key (id: 0x%08x) returned %d\n",
				    blob_id, status);
		if ((status != PSA_SUCCESS) &&
		    (status != PSA_ERROR_INVALID_HANDLE)) {
			OEM_PROV_PRINTF("Object (id: 0x%08x) import: FAILED\n",
					blob_id);
			return OEM_PROV_STATUS_PSA_ERROR;
		}
		/* In case the ID was not found in the key storage, it is still
		 * possible that ID to be present in the data storage. Try to
		 * delete the ID from the data storage
		 */
		if (status == PSA_ERROR_INVALID_HANDLE) {
			status = psa_its_remove(blob_id);
			OEM_PROV_DBG_PRINTF(DEBUG,
					    "psa_its_remove (id: 0x%08x) returned %d\n",
					     blob_id, status);
			if ((status != PSA_SUCCESS) &&
			    (status != PSA_ERROR_DOES_NOT_EXIST)) {
				OEM_PROV_PRINTF("Object (id: 0x%08x) import: FAILED\n",
						blob_id);
				return OEM_PROV_STATUS_PSA_ERROR;
			}
		}
	}

	/* The EL2GO_PROV_OEM_KEY can be imported only in OPEN lifecycle */
	if (blob_id == EL2GO_PROV_OEM_KEY_ID) {
		smw_lifecycle_t lifecycle = SMW_LIFECYCLE_NAME_NONE;
		int app_status = OEM_PROV_STATUS_OK;

		app_status = oem_prov_get_lifecycle(&lifecycle);
		if (app_status != OEM_PROV_STATUS_OK)
			return OEM_PROV_STATUS_SMW_ERROR;
		if (lifecycle != SMW_LIFECYCLE_NAME_OPEN) {
			OEM_PROV_DBG_PRINTF(ERROR,
					    "The OEM key can only be imported in open state\n");
			return OEM_PROV_STATUS_INVALID_LIFECYCLE;
		}
	}

	ret_blob_id = PSA_KEY_ID_NULL;
	status = psa_import_key(attributes, data, length, &ret_blob_id);
	OEM_PROV_DBG_PRINTF(DEBUG, "psa_import_key (id: 0x%08x) returned %d\n",
			    blob_id, status);

	if ((status != PSA_SUCCESS) && (blob_id == SRKH_KEY_ID)) {
		OEM_PROV_PRINTF("Object (id: 0x%08x) import: SRKH already imported\n",
				blob_id);
		return OEM_PROV_STATUS_OK;
	}

	if (status != PSA_SUCCESS) {
		OEM_PROV_PRINTF("Object (id: 0x%08x) import: FAILED\n",
				blob_id);
		return OEM_PROV_STATUS_PSA_ERROR;
	}

	OEM_PROV_PRINTF("Object (id: 0x%08x) import: SUCCESS\n", ret_blob_id);
	return OEM_PROV_STATUS_OK;
}
