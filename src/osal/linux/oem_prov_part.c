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
#include "oem_prov_arithmetic_ops.h"

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
			size_t len = strlen(mnt->mnt_dir), total_len = 0;

			if (ADD_OVERFLOW(len, 1, &total_len)) {
				status = OEM_PROV_STATUS_NUMBER_TOO_LARGE;
				goto exit;
			}
			*mount_point = malloc(total_len);
			if (!(*mount_point)) {
				status = OEM_PROV_STATUS_ALLOCATION_ERROR;
				goto exit;
			}
			memcpy(*mount_point, mnt->mnt_dir, total_len);
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
	size_t buff_len = 0;
	int status = OEM_PROV_STATUS_OK, ret = 0;
	char *copy_path = NULL, *ptr = NULL;

	if (path_len == 0 || path_len >= PATH_MAX) {
		OEM_PROV_DBG_PRINTF(ERROR, "Incorrect path size: %d\n",
				    path_len);
		status = OEM_PROV_STATUS_INVALID_FILE;
		goto exit;
	}
	if (ADD_OVERFLOW(path_len, 2, &buff_len)) {
		status = OEM_PROV_STATUS_NUMBER_TOO_LARGE;
		goto exit;
	}
	copy_path = (char *)malloc(buff_len);
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
	if (os_ctx->oem_config->indirect->needs_unmount) {
		if (umount(mount_point)) {
			OEM_PROV_DBG_PRINTF(ERROR,
					    "Umount returned %s for %s\n",
					    strerror(errno), mount_point);
			return OEM_PROV_STATUS_UMOUNT_ERROR;
		}
	}
	return OEM_PROV_STATUS_OK;
}

int oem_prov_load_env(char **path)
{
	int status = OEM_PROV_STATUS_OK;
	size_t len = 0;
	struct oem_prov_os_ctx *os_ctx = NULL;
	struct oem_prov_indirect *assets = NULL;

	os_ctx = oem_prov_get_os_ctx();
	OEM_PROV_DBG_ASSERT(os_ctx && os_ctx->oem_config &&
			    os_ctx->oem_config->indirect);
	assets = os_ctx->oem_config->indirect;

	/* partition cannot be empty string */
	if (!assets->partition || !strlen(assets->partition)) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid partition name\n");
		status = OEM_PROV_STATUS_INVALID_POINTER;
		goto exit;
	}

	status = is_device_mounted(assets->partition, path);
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	if (!*path) {
		if (!assets->mount_point || !assets->type) {
			status = OEM_PROV_STATUS_INVALID_POINTER;
			goto exit;
		}
		/* mount the device */
		status = mount_device(assets->partition, assets->mount_point,
				      assets->type);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;

		/* the device was mounted by the application, flag the necessity
		 * of unmounting it
		 */
		assets->needs_unmount = 1;
		len = strlen(assets->mount_point);
		if (INC_OVERFLOW(len, 1)) {
			status = OEM_PROV_STATUS_NUMBER_TOO_LARGE;
			unmount_device(os_ctx, assets->mount_point);
			goto exit;
		}
		*path = malloc(len);
		if (!(*path)) {
			status = OEM_PROV_STATUS_ALLOCATION_ERROR;
			unmount_device(os_ctx, assets->mount_point);
			goto exit;
		}
		memcpy(*path, assets->mount_point, len);
	}

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_load_assets(unsigned int index, char *path, void **stream)
{
	int status = OEM_PROV_STATUS_OK;
	size_t total_len = 0;
	int chars_written = 0;
	struct oem_prov_os_ctx *os_ctx = NULL;
	struct oem_prov_indirect *assets = NULL;
	FILE *fp = NULL;
	size_t file_length = 0;

	/* the path should not be NULL, the path points to the folder
	 * where the assets file is located. The path is an absolute
	 * path.
	 */
	if (!path) {
		status = OEM_PROV_STATUS_INVALID_POINTER;
		goto exit;
	}

	os_ctx = oem_prov_get_os_ctx();
	OEM_PROV_DBG_ASSERT(os_ctx && os_ctx->oem_config &&
			    os_ctx->oem_config->indirect);
	assets = os_ctx->oem_config->indirect;

	if (index >= assets->file_name_count) {
		status = OEM_PROV_STATUS_OUT_OF_BOUNDS;
		goto exit;
	}

	if (!assets->file_name || !assets->file_name[index]) {
		status = OEM_PROV_STATUS_INVALID_POINTER;
		goto exit;
	}

	/* the filename cannot be empty string or too long */
	file_length = strlen(assets->file_name[index]);
	if (!file_length || file_length >= NAME_MAX) {
		OEM_PROV_DBG_PRINTF(ERROR, "Incorrect file name size: %d\n",
				    file_length);
		status = OEM_PROV_STATUS_INVALID_FILE;
		goto exit;
	}

	/* path + '/' + file name + '\0' */
	if (ADD_OVERFLOW(file_length, strlen(path), &total_len)) {
		status = OEM_PROV_STATUS_NUMBER_TOO_LARGE;
		goto exit;
	}

	if (INC_OVERFLOW(total_len, 2)) {
		status = OEM_PROV_STATUS_NUMBER_TOO_LARGE;
		goto exit;
	}

	assets->assets_file_path = malloc(total_len);
	if (!assets->assets_file_path) {
		status = OEM_PROV_STATUS_ALLOCATION_ERROR;
		goto exit;
	}

	chars_written = snprintf(assets->assets_file_path, total_len, "%s/%s",
				 path, assets->file_name[index]);

	if (chars_written < total_len - 1) {
		free(assets->assets_file_path);
		status = OEM_PROV_STATUS_INCOMPLETE_DATA;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(INFO, "Assets file: %s\n",
			    assets->assets_file_path);
	fp = fopen(assets->assets_file_path, "rb");
	if (!fp) {
		OEM_PROV_DBG_PRINTF(ERROR, "Error opening file %s\n",
				    assets->assets_file_path);
		status = OEM_PROV_STATUS_INVALID_FILE;
		free(assets->assets_file_path);
		goto exit;
	}
	*stream = fp;
exit:

	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_unload_assets(void *stream)
{
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_indirect *assets = NULL;
	struct oem_prov_os_ctx *os_ctx = NULL;
	FILE *fp = (FILE *)stream;

	if (fp && fclose(fp))
		status = OEM_PROV_STATUS_INVALID_FILE;

	os_ctx = oem_prov_get_os_ctx();
	if (!os_ctx || !os_ctx->oem_config || !os_ctx->oem_config->indirect)
		return OEM_PROV_STATUS_INVALID_POINTER;

	assets = os_ctx->oem_config->indirect;

	/* delete the file if configured to be deleted */
	if (os_ctx->oem_config->indirect->delete_assets) {
		if (remove(assets->assets_file_path) < 0)
			status = OEM_PROV_STATUS_INVALID_FILE;
	}
	free(assets->assets_file_path);

	return status;
}

int oem_prov_unload_env(char **path)
{
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_indirect *assets = NULL;
	struct oem_prov_os_ctx *os_ctx = NULL;

	free(*path);
	*path = NULL;

	os_ctx = oem_prov_get_os_ctx();

	if (!os_ctx || !os_ctx->oem_config || !os_ctx->oem_config->indirect)
		return OEM_PROV_STATUS_INVALID_POINTER;
	assets = os_ctx->oem_config->indirect;

	if (assets->needs_unmount)
		status = unmount_device(os_ctx, assets->mount_point);

	return status;
}
