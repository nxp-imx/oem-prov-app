/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */

#ifndef __OEM_PROV_INTERNAL_H__
#define __OEM_PROV_INTERNAL_H__

struct oem_prov_online {
	char *hostname;
	char *port;
	char *server_cert;
	int close;
	int commit_storage;
};

struct oem_prov_indirect {
	char *partition;
	char *type;
	char *mount_point;
	char **file_name;
	unsigned int file_name_count;
	int delete_assets;
	int close;
	int commit_storage;
	int needs_unmount;
	char *assets_file_path;
};

struct oem_prov_config {
	struct oem_prov_online *online;
	struct oem_prov_indirect *indirect;
};

struct oem_prov_os_ctx {
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

#endif /* __OEM_PROV_INTERNAL_H__ */
