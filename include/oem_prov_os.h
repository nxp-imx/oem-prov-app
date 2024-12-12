/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */
#ifndef __OEM_PROV_OS_H__
#define __OEM_PROV_OS_H__

/**
 * oem_prov_get_buffer_from_file() - Reads the data from a file
 *
 * This function reads the content of a text file in a buffer. The function trims any
 * trailing spaces and line separators.
 *
 * @file_name: The file name
 * @buffer: The buffer where the data is stored. The buffer is allocated
 *          by this function and it should be freed by the caller.
 * @length: The size of the buffer. This is an output parameter, this function sets it.
 *
 * Return:
 * none
 */
int oem_prov_get_buffer_from_file(const char *file_name, unsigned char **buffer,
				  size_t *length);

/**
 * oem_prov_load_config_file() - Loads the configuration file by the Cyaml library
 * @file_name: The configuration file name
 * @option: Indicates if the user has selected online or indirect flow
 *
 * This function opens the configuration file and loads it into the Cyaml internal
 * structures.
 *
 * Return:
 * error code
 */
int oem_prov_load_config(const char *file_name, int option);

/**
 * oem_prov_unload_config() - Unloads the configuration file
 *
 * This functions frees the internal structures used by the Cyaml library.
 *
 * Return:
 * none
 */
void oem_prov_unload_config(void);

/**
 * oem_prov_config_set_host_and_port() - Sets EdgeLock 2GO server hostname and port
 *
 * This function sets the environment variables EDGELOCK2GO_HOSTNAME and
 * EDGELOCK2GO_PORT. The environment variables are used by the EdgeLock 2GO
 * agent libraries to connect to the EdgeLock 2GO server.
 *
 * Return:
 * none
 */
void oem_prov_set_host_and_port(void);

/**
 * oem_prov_load_assets() - Returns a stream associated with the security assets
 * @stream: Pointer to the stream (file or memory buffer) associated with the
 * security assets. This is used to read the content of security assets. This is
 * an output parameter, this function sets it.
 *
 * Return:
 * error code
 */
int oem_prov_load_assets(void **stream);

/**
 * oem_prov_unload_assets() - Cleanup any allocated resources
 * @stream: The stream (file or memory buffer) associated with the security assets
 *
 * This function cleanup all resources of the stream associated with the
 * security assets.
 *
 * Return:
 * error code
 */
int oem_prov_unload_assets(void *stream);

/**
 * oem_prov_read_data() - Reads data from a stream
 * @buffer: Address of the buffer where the read data will be stored.
 * @size: Size of each element to be read.
 * @nmemb: Number of elements to be read.
 * @stream: Pointer to the stream (file or memory buffer) from which data is read.
 * The actual type of stream is determined by the implementation.
 *
 * Return:
 * The total number of elements successfully read.
 */
size_t oem_prov_read_data(void *buffer, size_t size, size_t nmemb,
			  void *stream);

/**
 * oem_prov_set_offset() - Moves the stream position indicator for a stream
 * @stream: Pointer to the stream (file or memory buffer) for which the position
 * is set. The actual type of stream is determined by the implementation.
 * @offset: Offset in bytes relative to the whence parameter.
 * @whence: The reference point for the offset.
 *
 * Return:
 * error code
 */
int oem_prov_set_offset(void *stream, long offset, int whence);

/**
 * oem_prov_get_offset() - Gets the stream position indicator of a stream
 * @stream: Pointer to the stream (file or memory buffer) for which the position
 * is set. The actual type of stream is determined by the implementation.
 *
 * Return:
 * The current position in the stream . Returns -1 if an error occurs.
 */
long oem_prov_get_offset(void *stream);

/**
 * oem_prov_get_lc_option() - Get the configuration options for the device lifecycle
 *
 * @mode: The current flow: online, indirect, etc.
 *
 * Return:
 * Returns the setting from the configuration file. After the provisioning is done,
 * there is an option to close the device. If the setting is missing from the
 * configuration file, no action is taken.
 *
 */
int oem_prov_get_lc_option(unsigned int mode);

/**
 * oem_prov_get_commit_storage() - Returns the user option for commit_storage
 *
 * After the provisioning is done (either online, indirect or batch mode),
 * the non-volatile key storage can be committed into the physical memory.
 * The configuration file has an option to indicate if the storage is to be
 * committed immediately after provisioning. This can be also done at a later stage
 * using a command line argument.
 *
 * @mode: the provisioning flow type: online, indirect or batch flow
 *
 * Return:
 * the option from the configuration file
 */
int oem_prov_get_commit_storage(unsigned int mode);

#endif /* __OEM_PROV_OS_H__ */
