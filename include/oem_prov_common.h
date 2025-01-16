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
 * @OEM_PROV_INDIRECT: Indirect mode, the provisioning is done without EdgeLock 2Go server
 */
enum oem_prov_config_options { OEM_PROV_ONLINE, OEM_PROV_INDIRECT };

enum oem_prov_lc_options {
	OEM_PROV_LC_NONE = 0,
	OEM_PROV_LC_CLOSED,
	OEM_PROV_LC_CLOSED_LOCKED
};

#endif /* __OEM_PROV_COMMON_H__ */
