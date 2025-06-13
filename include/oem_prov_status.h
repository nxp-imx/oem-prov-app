/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */
#ifndef __OEM_PROV_STATUS_H__
#define __OEM_PROV_STATUS_H__

/**
 * enum oem_prov_status_code - OEM Provisioning Application status codes
 *
 * @OEM_PROV_STATUS_OK: Function returned without any error
 * @OEM_PROV_STATUS_INVALID_FILE: The configuration file/claim code file cannot be opened
 * @OEM_PROV_STATUS_EMPTY_FILE: The configuration file is empty
 * @OEM_PROV_STATUS_ONLINE_OPT_MISSING: The online option from thr config file is missing
 * @OEM_PROV_STATUS_OFFLINE_OPT_MISSING: The offline option from the config file is missing
 * @OEM_PROV_STATUS_FILENAME_TOO_LONG: The file name is too long
 * @OEM_PROV_STATUS_INVALID_OPTION: Invalid option in the configuration file
 *
 * @OEM_PROV_STATUS_ALLOCATION_ERROR: Memory allocation error
 * @OEM_PROV_STATUS_INVALID_POINTER: A pointer is invalid
 * @OEM_PROV_STATUS_INVALID_STRING: The command line string is invalid
 *
 * @OEM_PROV_STATUS_SMW_ERROR: The SMW library returned an error
 * @OEM_PROV_STATUS_EL2GO_AGENT_ERROR: The EdgeLock 2GO agent libraries returned an error
 *
 * @OEM_PROV_STATUS_MKDIR_ERROR: The mkdir returned an error
 * @OEM_PROV_STATUS_MOUNT_ERROR: The mount operation failed
 * @OEM_PROV_STATUS_UMOUNT_ERROR: The unmount operation failed
 *
 * @OEM_PROV_STATUS_INCOMPLETE_DATA: The data in the file does not respect the expected format
 * @OEM_PROV_STATUS_PSA_ERROR: A function in the PSA API returned an error
 */
enum oem_prov_status_code {
	OEM_PROV_STATUS_OK = 0,
	OEM_PROV_STATUS_INVALID_FILE,
	OEM_PROV_STATUS_EMPTY_FILE,
	OEM_PROV_STATUS_ONLINE_OPT_MISSING,
	OEM_PROV_STATUS_OFFLINE_OPT_MISSING,
	OEM_PROV_STATUS_FILENAME_TOO_LONG,
	OEM_PROV_STATUS_INVALID_OPTION,
	OEM_PROV_STATUS_ALLOCATION_ERROR,
	OEM_PROV_STATUS_INVALID_POINTER,
	OEM_PROV_STATUS_INVALID_STRING,
	OEM_PROV_STATUS_SMW_ERROR,
	OEM_PROV_STATUS_EL2GO_AGENT_ERROR,
	OEM_PROV_STATUS_MKDIR_ERROR,
	OEM_PROV_STATUS_MOUNT_ERROR,
	OEM_PROV_STATUS_UMOUNT_ERROR,
	OEM_PROV_STATUS_INCOMPLETE_DATA,
	OEM_PROV_STATUS_PSA_ERROR,
	OEM_PROV_STATUS_NUMBER_TOO_LARGE,
	OEM_PROV_STATUS_OUT_OF_BOUNDS
};

#endif /* __OEM_PROV_STATUS_H__ */
