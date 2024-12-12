// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025 NXP
 */

#include <stdio.h>

size_t oem_prov_read_data(void *buffer, size_t size, size_t nmemb, void *stream)
{
	return fread(buffer, size, nmemb, stream);
}

int oem_prov_set_offset(void *stream, long offset, int whence)
{
	return fseek(stream, offset, whence);
}

long oem_prov_get_offset(void *stream)
{
	return ftell(stream);
}
