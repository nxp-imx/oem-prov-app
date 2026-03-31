// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2026 NXP
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_arithmetic_ops.h"
#include "oem_prov_os.h"

int oem_prov_get_buffer_from_file(const char *file_name, unsigned char **buffer,
				  unsigned int *length, unsigned int offset)
{
	int res = OEM_PROV_STATUS_OK;
	int fd = 0;
	struct stat st = { 0 };
	unsigned int file_size = 0;
	long pread_bytes = 0;
	unsigned int buff_length = 0;

	fd = open(file_name, O_RDONLY);
	if (fd == -1)
		return OEM_PROV_STATUS_INVALID_FILE;

	/* make sure that we have enough space to write file_size
	 * bytes
	 */
	if (offset > *length) {
		res = OEM_PROV_STATUS_OUT_OF_BOUNDS;
		goto exit;
	}

	if (((fstat(fd, &st) != 0)) || (!S_ISREG(st.st_mode))) {
		close(fd);
		return OEM_PROV_STATUS_INVALID_FILE;
	}
	if (st.st_size < 0) {
		res = OEM_PROV_STATUS_INVALID_FILE;
		goto exit;
	}
	file_size = (unsigned int)(st.st_size);

	if (*buffer) {
		res = OEM_PROV_STATUS_INVALID_POINTER;
		goto exit;
	}
	if (ADD_OVERFLOW(file_size, *length, &buff_length)) {
		res = OEM_PROV_STATUS_NUMBER_TOO_LARGE;
		goto exit;
	}

	*buffer = (unsigned char *)malloc(buff_length);
	if (!(*buffer)) {
		res = OEM_PROV_STATUS_ALLOCATION_ERROR;
		goto exit;
	}
	pread_bytes = pread(fd, *buffer + offset, file_size, 0);
	if (pread_bytes < file_size) {
		free(*buffer);
		*buffer = NULL;
		res = OEM_PROV_STATUS_INVALID_FILE;
	}
	*length = file_size;
exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, res);
	close(fd);
	return res;
}

int oem_prov_confirm_operation(const char *const *warning_lines,
			       unsigned int num_lines)
{
	char response[10] = { 0 };
	unsigned int i = 0;
	int res = OEM_PROV_STATUS_USER_ABORT;

	/* Check if confirmation is disabled */
	if (oem_prov_get_no_confirm()) {
		OEM_PROV_DBG_PRINTF(INFO, "Confirmation bypassed\n");
		res = OEM_PROV_STATUS_OK;
		goto exit;
	}

	OEM_PROV_PRINTF("\n");
	for (i = 0; i < num_lines; i++)
		OEM_PROV_PRINTF("%s\n", warning_lines[i]);

	OEM_PROV_PRINTF("\n");

	OEM_PROV_PRINTF("Are you sure you want to proceed? (yes/no): ");
	(void)fflush(stdout);

	if (!fgets(response, sizeof(response), stdin)) {
		OEM_PROV_DBG_PRINTF(ERROR, "Failed to read user input\n");
		goto exit;
	}

	/* Remove trailing newline */
	response[strcspn(response, "\n")] = '\0';

	if (strcmp(response, "yes") == 0)
		res = OEM_PROV_STATUS_OK;
	else
		OEM_PROV_PRINTF("Operation aborted by user.\n");

exit:
	return res;
}
