/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024 NXP
 */
#ifndef __OEM_PROV_STATUS_H__
#define __OEM_PROV_STATUS_H__

/**
 * enum oem_prov_status_code - OEM Provisioning Application status codes
 *
 * @OEM_PROV_STATUS_OK: Function returned without any error
 * @OEM_PROV_STATUS_INVALID_FILE: The configuration file/claim code file cannot be opened
 * @OEM_PROV_STATUS_ALLOCATION_ERROR: Memory allocation error
 * @OEM_PROV_STATUS_SMW_ERROR: The SMW library returned an error
 * @OEM_PROV_STATUS_EL2GO_AGENT_ERROR: The EdgeLock 2GO agent libraries returned an error
 */
enum oem_prov_status_code {
	OEM_PROV_STATUS_OK = 0,
	OEM_PROV_STATUS_INVALID_FILE,
	OEM_PROV_STATUS_FILENAME_TOO_LONG,
	OEM_PROV_STATUS_ALLOCATION_ERROR,
	OEM_PROV_STATUS_SMW_ERROR,
	OEM_PROV_STATUS_EL2GO_AGENT_ERROR,
};

#endif /* __OEM_PROV_STATUS_H__ */
