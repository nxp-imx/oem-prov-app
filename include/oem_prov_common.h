/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024 NXP
 */

#ifndef __OEM_PROV_COMMON_H__

/**
 * enum oem_prov_config_options - OEM Provisioning Application modes
 *
 * @OEM_PROV_ONLINE: Online mode, the provisioning is done through EdgeLock 2GO server
 * @OEM_PROV_INDIRECT: Indirect mode, the provisioning is done without EdgeLock 2Go server
 */
enum oem_prov_config_options { OEM_PROV_ONLINE, OEM_PROV_INDIRECT };

#endif /* __OEM_PROV_COMMON_H__ */
