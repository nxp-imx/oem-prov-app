/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024 NXP
 */
#ifndef __OEM_PROV_CONFIG_H__
#define __OEM_PROV_CONFIG_H__

/**
 * oem_load_config_file() - Loads the configuration file by the Cyaml library
 * @filename: The configuration file name
 *
 * This function opens the configuration file and loads it into the Cyaml internal
 * structures.
 *
 * Return:
 * error code
 */
int oem_load_config_file(const char *filename);

/**
 * oem_unload_config() - Unloads the configuration file
 *
 * This functions frees the internal structures used by the Cyaml library.
 *
 * Return:
 * none
 */
void oem_unload_config(void);

#endif /* __OEM_PROV_CONFIG_H__ */
