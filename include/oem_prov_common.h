/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */

#ifndef __OEM_PROV_COMMON_H__
#define __OEM_PROV_COMMON_H__

/**
 * enum oem_prov_config_options - OEM Provisioning Application modes
 *
 * @OEM_PROV_ONLINE: Online mode, the provisioning is done through EdgeLock 2GO server
 * @OEM_PROV_OFFLINE: Offline modes, the provisioning is done without EdgeLock 2GO server
 */
enum oem_prov_config_options { OEM_PROV_ONLINE, OEM_PROV_OFFLINE };

enum oem_prov_lc_options {
	OEM_PROV_LC_NONE = 0,
	OEM_PROV_LC_CLOSED,
	OEM_PROV_LC_CLOSED_LOCKED
};

enum oem_prov_storage_options {
	OEM_PROV_S_NONE = 0,
	OEM_PROV_S_COMMIT
};

enum oem_prov_bool_options {
	OEM_PROV_FALSE = 0,
	OEM_PROV_TRUE
};

/**
 * oem_prov_get_uint_be() - Extracts a big-endian unsigned integer
 *
 * This function takes an address and reads a given number of consecutive bytes, treating
 * them as a big-endian integer. The first byte is the most significant byte, and the last
 * is the least significant byte.
 *
 * @buffer: The buffer containing the bytes.
 * @bytes: The number of consecutive bytes.
 *
 * Return:
 * An unsigned integer representing the big-endian value read from the buffer.
 */
unsigned int oem_prov_get_uint_be(const unsigned char *buffer,
				  unsigned int bytes);

#endif /* __OEM_PROV_COMMON_H__ */
