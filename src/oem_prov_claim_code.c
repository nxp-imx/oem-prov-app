// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 NXP
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <smw_storage.h>
#include <smw_keymgr.h>
#include <smw_status.h>

#include "oem_prov.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

#define OEM_PROV_CLAIM_CODE_ID	       0xF00000E0
#define OEM_PROV_CLAIM_ENC_KEY_ID      0x7FFF8171
#define OEM_PROV_CLAIM_AUTH_KEY_ID     0x7FFF8172

/**
 * get_buffer_from_file() - Reads the data from a file
 *
 * This function reads the claim code from a file. The function trims any
 * trailing spaces and line separators.
 *
 * @file_name: The file name where the claim code is.
 * @buffer: The buffer where the claim code data is stored. The buffer is allocated
 *          by this function and it should be freed by the caller.
 * @length: The size of the buffer. This is an output parameter, this function sets it.
 *
 * Return:
 * none
 */
static int get_buffer_from_file(const char *file_name, unsigned char **buffer,
				size_t *length)
{
	int res = OEM_PROV_STATUS_OK;
	int fd;
	struct stat st;
	size_t file_size;
	int i;

	fd = open(file_name, O_RDONLY);
	if (fd == -1)
		return OEM_PROV_STATUS_INVALID_FILE;

	if (((fstat(fd, &st) != 0)) || (!S_ISREG(st.st_mode))) {
		close(fd);
		return OEM_PROV_STATUS_INVALID_FILE;
	}

	file_size = st.st_size;
	*buffer = (char *)malloc(file_size);
	if (*buffer == NULL) {
		res = OEM_PROV_STATUS_ALLOCATION_ERROR;
		goto exit;
	}

	if (file_size != pread(fd, *buffer, file_size, 0)) {
		free(*buffer);
		*buffer = NULL;
		res = OEM_PROV_STATUS_INVALID_FILE;
		goto exit;
	}

	/* trim any trailing characters */
	for (i = file_size - 1; i >= 0; i--) {
		if ((*buffer)[i] == ' ' || (*buffer)[i] == '\r' ||
		    (*buffer)[i] == '\n') {
			continue;
		} else {
			break;
		}
	}
	*length = i + 1;
	(*buffer)[*length] = '\0';

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, res);
	close(fd);
	return res;
}

int oem_prov_inject_claimcode(const char *filename)
{
	int status = OEM_PROV_STATUS_OK;
	unsigned char *cc;
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

	status = get_buffer_from_file(filename, &cc, &buffer_length);
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
