// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

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
int oem_prov_get_buffer_from_file(const char *file_name, unsigned char **buffer,
				  size_t *length)
{
	int res = OEM_PROV_STATUS_OK;
	int fd = 0;
	struct stat st = { 0 };
	size_t file_size = 0;
	int i = 0;

	fd = open(file_name, O_RDONLY);
	if (fd == -1)
		return OEM_PROV_STATUS_INVALID_FILE;

	if (((fstat(fd, &st) != 0)) || (!S_ISREG(st.st_mode))) {
		close(fd);
		return OEM_PROV_STATUS_INVALID_FILE;
	}

	file_size = st.st_size;

	if (*buffer) {
		res = OEM_PROV_STATUS_INVALID_POINTER;
		goto exit;
	}
	*buffer = (char *)malloc(file_size);
	if (!(*buffer)) {
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
