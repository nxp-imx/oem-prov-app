/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */

#ifndef __OEM_PROV_INTERNAL_H__
#define __OEM_PROV_INTERNAL_H__

struct oem_prov_online {
	char *hostname;
	char *port;
	int close;
};

struct oem_prov_indirect {
	char *partition;
	char *type;
	char *mount_point;
	char *file_name;
	int close;
};

struct oem_prov_config {
	struct oem_prov_online *online;
	struct oem_prov_indirect *indirect;
};

struct oem_prov_os_ctx {
	int needs_unmount;
	struct oem_prov_config *oem_config;
};

/**
 * oem_prov_os_ctx() - Get internal context
 *
 * This function returns the internal context for the OS related
 * functions layer. The context contains state and configuration variables.
 *
 * Return:
 * context pointer
 */
struct oem_prov_os_ctx *oem_prov_get_os_ctx(void);

/**
 * oem_prov_load_assets_file() -Returns the name of the assets file
 * @file_path: Returns the full path of the file containing the security objects
 *
 * The function checks if the device where the security assets file is located
 * is already mounted. If it is already mounted, the complete file path is returned.
 * If the device is not mounted, it is mounted, and the full file path returned.
 * At the end the device is mounted if it was mounted by the application.
 *
 * Return:
 * error code
 */
int oem_prov_load_assets_file(char **file_path);

/**
 * oem_prov_unload_assets_file() - Cleanup any allocated resources
 *
 * If the device was mounted, unmount the device.
 *
 * Return:
 * error code
 */
int oem_prov_unload_assets_file(void);

#endif /* __OEM_PROV_INTERNAL_H__ */
