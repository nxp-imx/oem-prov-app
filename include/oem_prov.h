/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024 NXP
 */

#ifndef __OEM_PROV_H__
#define __OEM_PROV_H__

/**
 * oem_prov_get_uuid() - Returns the device UUID
 *
 * This function reads the device UUID using the SMW library and
 * prints it in hex format.
 *
 * Return:
 * error code
 */
int oem_prov_get_uuid(void);

/**
 * oem_prov_init_smw() - Initialize the SMW library
 *
 * This function initializes the SMW library in order to be used by the
 * EdgeLock 2GO application.
 *
 * Return:
 * none
 */
int oem_prov_init_smw(void);

/**
 * oem_prov_inject_claimcode() - Injects the claim code in the device
 *
 * @filename: The file name where the claim code is stored. The claim code is
 * in base64 format as it is exported by the EdgeLock 2GO server.
 *
 * Return:
 * error code
 */
int oem_prov_inject_claimcode(const char *filename);

/**
 * oem_prov_online() - Runs the OEM Provisioning Application in online mode.
 *
 * This function uses the EdgeLock 2GO Agent libraries to connect to EdgeLock 2GO server,
 * to download the security assets and to provision the device with the downloaded security
 * assets.
 *
 * Return:
 * error code
 */
int oem_prov_online(void);

#endif /* __OEM_PROV_H__ */
