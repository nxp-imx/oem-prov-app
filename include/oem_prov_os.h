/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024 NXP
 */
#ifndef __OEM_PROV_OS_H__
#define __OEM_PROV_OS_H__

/**
 * get_buffer_from_file() - Reads the data from a file
 *
 * This function reads the claim code from a file. The function trims any
 * trailing spaces and line separators.
 *
 * @file_name: The file name where the claim code is.
 * @buffer: The buffer where the claim code data is stored. The buffer is allocated
 *          by this function and it should be freed by the caller.
 * @length: The size of the buffer. This is an output parameter, this function sets it.
 *
 * Return:
 * none
 */
int oem_prov_get_buffer_from_file(const char *file_name, unsigned char **buffer,
				  size_t *length);

/**
 * oem_load_config_file() - Loads the configuration file by the Cyaml library
 * @filename: The configuration file name
 * @option: Indicates if the user has selected online or indirect flow
 *
 * This function opens the configuration file and loads it into the Cyaml internal
 * structures.
 *
 * Return:
 * error code
 */
int oem_prov_load_config(const char *filename, int option);

/**
 * oem_unload_config() - Unloads the configuration file
 *
 * This functions frees the internal structures used by the Cyaml library.
 *
 * Return:
 * none
 */
void oem_prov_unload_config(void);

/**
 * oem_config_set_host_and_port() - Sets EdgeLock 2GO server hostname and port
 *
 * This function sets the environment variables EDGELOCK2GO_HOSTNAME and
 * EDGELOCK2GO_PORT. The environment variables are used by the EdgeLock 2GO
 * agent libraries to connect to the EdgeLock 2GO server.
 *
 * Return:
 * none
 */
void oem_prov_set_host_and_port(void);

#endif /* __OEM_PROV_OS_H__ */
