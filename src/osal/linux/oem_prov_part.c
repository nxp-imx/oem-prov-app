// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */

#include <mntent.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

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
	struct mntent *mnt = NULL;
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
			size_t len = strlen(mnt->mnt_dir);

			*mount_point = malloc(len + 1);
			if (!(*mount_point)) {
				status = OEM_PROV_STATUS_ALLOCATION_ERROR;
				goto exit;
			}
			strncpy(*mount_point, mnt->mnt_dir, len);
			(*mount_point)[len] = '\0';
			break;
		}
	}

exit:
	endmntent(f);
	return status;
}

/**
 * mkdir_p() - Creates a directory and all necessary parent directories
 * @path: The directory path to create
 * @mode: The permissions to set on created directory
 *
 * The function recursively creates a directory and all parent directories if
 * they do not exist.
 * Return:
 * error code
 */
static int mkdir_p(const char *path, unsigned int mode)
{
	size_t path_len = strlen(path);
	int status = OEM_PROV_STATUS_OK, ret = 0;
	char *copy_path = NULL, *ptr = NULL;

	if (path_len == 0 || path_len > PATH_MAX - 1) {
		status = OEM_PROV_STATUS_INVALID_FILE;
		goto exit;
	}

	copy_path = (char *)malloc(path_len + 2);
	if (!copy_path) {
		status = OEM_PROV_STATUS_ALLOCATION_ERROR;
		goto exit;
	}

	strcpy(copy_path, path);
	if (copy_path[path_len - 1] != '/') {
		copy_path[path_len] = '/';
		copy_path[path_len + 1] = '\0';
	}

	for (ptr = copy_path + 1; *ptr; ptr++) {
		if (*ptr == '/') {
			*ptr = '\0';
			ret = mkdir(copy_path, 0755);
			/* Check if the error is due to folder already existing */
			if (ret && (errno != EEXIST)) {
				OEM_PROV_DBG_PRINTF(ERROR,
						    "mkdir returned %s for dir %s\n",
						    strerror(errno), copy_path);
				status = OEM_PROV_STATUS_MKDIR_ERROR;
				*ptr = '/';
				goto close;
			}

			/* Check if the path exists, but is not a directory */
			ret = open(copy_path,
				   O_RDONLY | O_DIRECTORY | O_NOFOLLOW);
			if (ret == -1) {
				OEM_PROV_DBG_PRINTF(ERROR,
						    "%s already exists, but is not a directory\n",
						    copy_path);
				status = OEM_PROV_STATUS_MKDIR_ERROR;
				*ptr = '/';
				goto close;
			}
			close(ret);
			*ptr = '/';
		}
	}

close:
	free(copy_path);
exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

/**
 * mount_device() - Mounts the device
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

	status = mkdir_p(mount_point, 0755);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

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

static inline int unmount_device(struct oem_prov_os_ctx *os_ctx,
				 char *mount_point)
{
	if (os_ctx->needs_unmount) {
		if (umount(mount_point)) {
			OEM_PROV_DBG_PRINTF(ERROR,
					    "Umount returned %s for %s\n",
					    strerror(errno), mount_point);
			return OEM_PROV_STATUS_UMOUNT_ERROR;
		}
	}
	return OEM_PROV_STATUS_OK;
}

int oem_prov_load_assets(void **stream)
{
	int status = OEM_PROV_STATUS_OK;
	char *mount_point = NULL;
	size_t total_len = 0;
	int chars_written = 0;
	char *mount_point_ptr = NULL;
	struct oem_prov_os_ctx *os_ctx = NULL;
	struct oem_prov_indirect *assets = NULL;
	char *file_path = NULL;
	FILE *fp = NULL;
	int test_unmount = 1;

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
	file_path = malloc(total_len);
	if (!file_path) {
		status = OEM_PROV_STATUS_ALLOCATION_ERROR;
		goto exit;
	}

	chars_written = snprintf(file_path, total_len, "%s/%s", mount_point_ptr,
				 assets->file_name);

	if (chars_written < total_len - 1) {
		status = OEM_PROV_STATUS_INCOMPLETE_DATA;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(INFO, "Assets file is %s\n", file_path);
	fp = fopen(file_path, "rb");
	if (!fp) {
		OEM_PROV_DBG_PRINTF(ERROR, "Error opening file %s\n",
				    file_path);
		status = OEM_PROV_STATUS_INVALID_FILE;
		goto exit;
	}
	*stream = fp;
	test_unmount = 0;
exit:
	/* if the device was already mounted, free the allocated mount point */
	if (mount_point)
		free(mount_point);

	if (test_unmount)
		unmount_device(os_ctx, assets->mount_point);

	free(file_path);
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_unload_assets(void *stream)
{
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_indirect *assets = NULL;
	struct oem_prov_os_ctx *os_ctx = NULL;
	FILE *fp = (FILE *)stream;

	os_ctx = oem_prov_get_os_ctx();
	OEM_PROV_DBG_ASSERT(os_ctx && os_ctx->oem_config &&
			    os_ctx->oem_config->indirect);
	assets = os_ctx->oem_config->indirect;

	if (fclose(fp))
		status = OEM_PROV_STATUS_INVALID_FILE;

	if (os_ctx->needs_unmount) {
		status = unmount_device(os_ctx, assets->mount_point);
	}

	return status;
}
