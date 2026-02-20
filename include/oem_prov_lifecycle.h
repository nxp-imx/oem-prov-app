/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2026 NXP
 */

#ifndef __OEM_PROV_LIFECYCLE_H__
#define __OEM_PROV_LIFECYCLE_H__

#include <smw_device.h>

/**
 * oem_prov_read_lifecycle() - Reads the device lifecycle
 *
 * This function reads the device lifecycle using the SMW library.
 *
 * @lifecycle: Pointer to store the lifecycle value
 *
 * Return:
 * error code
 */
int oem_prov_read_lifecycle(smw_lifecycle_t *lifecycle);

#endif /* __OEM_PROV_LIFECYCLE_H__ */
