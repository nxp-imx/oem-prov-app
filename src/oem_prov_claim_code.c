// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 NXP
 */

#include <smw_storage.h>
#include <smw_keymgr.h>
#include <smw_status.h>

#include "oem_prov.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"

#define OEM_PROV_CLAIM_CODE_ID	       0xF00000E0
#define OEM_PROV_CLAIM_ENC_KEY_ID      0x7FFF8171
#define OEM_PROV_CLAIM_AUTH_KEY_ID     0x7FFF8172

int oem_prov_inject_claimcode(const char *filename)
{
	int status = OEM_PROV_STATUS_OK;
	unsigned char *cc = NULL;
	size_t buffer_length;
	enum smw_status_code smw_status;

	struct smw_store_data_args args = { 0 };
	struct smw_data_descriptor data_descriptor = { 0 };
	struct smw_encryption_args encr_args = { 0 };
	struct smw_sign_args sign_args = { 0 };
	struct smw_data_attributes data_attrs = { 0 };

	struct smw_key_descriptor encr_key = { 0 };
	struct smw_key_descriptor *encr_key_ptr = &encr_key;
	struct smw_key_descriptor sign_key = { 0 };

	status = oem_prov_get_buffer_from_file(filename, &cc, &buffer_length);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Config file read failed %d\n",
				    status);
		goto exit;
	}

	data_descriptor.identifier = OEM_PROV_CLAIM_CODE_ID;
	data_descriptor.data = cc;
	data_descriptor.length = buffer_length;
	data_descriptor.data_attributes = &data_attrs;
	SMW_ATTR_SET_PERSISTENT(data_attrs.attributes);
	SMW_ATTR_SET_LC_CURRENT(data_attrs.attributes);

	encr_key.type_name = SMW_KEY_TYPE_NAME_AES;
	encr_key.id = OEM_PROV_CLAIM_ENC_KEY_ID;
	encr_key.security_size = 256;

	encr_args.nb_keys = 1;
	encr_args.mode_name = SMW_CIPHER_MODE_NAME_CBC;
	encr_args.keys_desc = &encr_key_ptr;

	sign_key.type_name = SMW_KEY_TYPE_NAME_AES;
	sign_key.id = OEM_PROV_CLAIM_AUTH_KEY_ID;
	sign_key.security_size = 256;

	sign_args.algo_name = SMW_MAC_ALGO_NAME_CMAC;
	sign_args.key_descriptor = &sign_key;

	args.data_descriptor = &data_descriptor;
	args.encryption_args = &encr_args;
	args.sign_args = &sign_args;
	args.subsystem_name = SMW_SUBSYSTEM_NAME_ELE;

	smw_status = smw_store_data(&args);
	if (smw_status != SMW_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "SMW store failed %d\n", status);
		status = OEM_PROV_STATUS_SMW_ERROR;
	}

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	if (cc != NULL)
		free(cc);
	/* remove the file from the disk */
	remove(filename);
	return status;
}
