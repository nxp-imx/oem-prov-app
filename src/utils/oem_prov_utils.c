// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */

#include "oem_prov_common.h"

unsigned int oem_prov_get_uint_be(const unsigned char *buffer,
				  unsigned int bytes)
{
	unsigned int value = 0U, i = 0;

	value = *(buffer);

	for (i = 1; i < bytes; i++) {
		value <<= 8;
		value |= *(buffer + i);
	}

	return value;
}
