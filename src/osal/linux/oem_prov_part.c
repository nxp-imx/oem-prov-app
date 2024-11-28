// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 NXP
 */

#include <mntent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>
#include <errno.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_internal.h"

#define MOUNT_FILE "/proc/mounts"

/**
 * is_device_mounted() - Check if the device is mounted
 * @device_name: the device name as seen by the fdisk utility
 * @mount_point: returns the mount point
 *
 * The function checks if the device is mounted. If it is mounted, it
 * returns the mount point or NULL if the device is not mounted.
 * Return:
 * error code
 */
static int is_device_mounted(const char *device_name, char **mount_point)
{
	struct mntent *mnt;
	FILE *f = NULL;
	int status = OEM_PROV_STATUS_OK;
	*mount_point = NULL;

	f = setmntent(MOUNT_FILE, "r");
	if (!f) {
		OEM_PROV_DBG_PRINTF(VERBOSE, "Reading %s returned %s\n",
				    MOUNT_FILE, strerror(errno));
		return OEM_PROV_STATUS_INVALID_FILE;
	}
	while ((mnt = getmntent(f)) != NULL) {
		if (strcmp(device_name, mnt->mnt_fsname) == 0) {
			int len;

			len = strlen(mnt->mnt_dir);
			*mount_point = malloc(len + 1);
			if (!(*mount_point)) {
				status = OEM_PROV_STATUS_ALLOCATION_ERROR;
				goto exit;
			}
			strncpy(*mount_point, mnt->mnt_dir, len);
			mount_point[len] = '\0';
		}
	}
exit:
	endmntent(f);
	return status;
}

/**
 * mount_device() - Check if the device is mounted
 * @device: the device name as seen by the fdisk utility
 * @mount_point: the mount point
 * @fs_type: file system type
 *
 * The function mounts the device in the specified mount point.
 * Return:
 * error code
 */
static int mount_device(const char *device, const char *mount_point,
			const char *fs_type)
{
	int status = OEM_PROV_STATUS_OK;
	struct stat st;

	/* Attempt to create the directory */
	if (mkdir(mount_point, 0755) != 0) {
		/* check if the error is due to folder already existing */
		if (errno != EEXIST) {
			OEM_PROV_DBG_PRINTF(ERROR,
					    "Mkdir returned %s for dir %s\n",
					    strerror(errno), mount_point);
			status = OEM_PROV_STATUS_MKDIR_ERROR;
			goto exit;
		}
	}
	if (!stat(mount_point, &st)) {
		if (!S_ISDIR(st.st_mode)) {
			/* if the file exists, but is not a directory */
			status = OEM_PROV_STATUS_MKDIR_ERROR;
			goto exit;
		}
	}
	status = mount(device, mount_point, fs_type, 0, NULL);
	if (status != 0) {
		OEM_PROV_DBG_PRINTF(ERROR, "Mount returned %s for device %s\n",
				    strerror(errno), device);
		status = OEM_PROV_STATUS_MOUNT_ERROR;
		goto exit;
	}
exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_load_assets_file(char **file_path)
{
	int status = OEM_PROV_STATUS_OK;
	char *mount_point = NULL;
	int total_len = 0;
	char *mount_point_ptr = NULL;
	struct oem_prov_os_ctx *os_ctx;
	struct oem_prov_indirect *assets;

	os_ctx = oem_prov_get_os_ctx();

	OEM_PROV_DBG_ASSERT(os_ctx && os_ctx->oem_config &&
			    os_ctx->oem_config->indirect);
	assets = os_ctx->oem_config->indirect;
	status = is_device_mounted(assets->partition, &mount_point);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	if (mount_point) {
		mount_point_ptr = mount_point;
	} else {
		/* mount the device */
		status = mount_device(assets->partition, assets->mount_point,
				      assets->type);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
		/* the device was mounted by the application, flag the necessity
		 * of unmounting it
		 */
		os_ctx->needs_unmount = 1;
		mount_point_ptr = assets->mount_point;
	}
	/* path + file name + '/' + '\0' */
	total_len = strlen(assets->file_name) + strlen(mount_point_ptr) + 2;
	*file_path = malloc(total_len);
	snprintf(*file_path, total_len, "%s/%s", mount_point_ptr,
		 assets->file_name);
	/* if the device was already mounted, free the allocated mount point */
	if (mount_point)
		free(mount_point);

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_unload_assets_file(void)
{
	struct oem_prov_indirect *assets;
	struct oem_prov_os_ctx *os_ctx;

	os_ctx = oem_prov_get_os_ctx();
	OEM_PROV_DBG_ASSERT(os_ctx && os_ctx->oem_config &&
			    os_ctx->oem_config->indirect);
	assets = os_ctx->oem_config->indirect;

	if (os_ctx->needs_unmount) {
		if (umount(assets->mount_point)) {
			OEM_PROV_DBG_PRINTF(ERROR,
					    "Umount returned %s for %s\n",
					    strerror(errno),
					    assets->mount_point);
			return OEM_PROV_STATUS_UMOUNT_ERROR;
		}
	}
	return OEM_PROV_STATUS_OK;
}
