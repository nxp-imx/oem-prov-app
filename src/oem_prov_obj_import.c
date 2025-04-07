// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025 NXP
 */

#include "oem_prov_blob.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "psa/crypto.h"

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
	 */
	if (blob_id != SRKH_KEY_ID) {
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
