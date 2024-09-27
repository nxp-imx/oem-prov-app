/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024 NXP
 */
#ifndef __OEM_PROV_DEBUG_INFO_H__
#define __OEM_PROV_DEBUG_INFO_H__

#include <stdio.h>
#include <stdlib.h>

/* Debug levels*/
#define OEM_PROV_DBG_LEVEL_NONE    0 /* No debug information */
#define OEM_PROV_DBG_LEVEL_ERROR   1 /* Important error for the user*/
#define OEM_PROV_DBG_LEVEL_INFO    2 /* First level of debugging information */
#define OEM_PROV_DBG_LEVEL_DEBUG   3 /* Second level of debuggin information */
#define OEM_PROV_DBG_LEVEL_VERBOSE 4 /* Maximum level of debugging information */

#if defined(ENABLE_VERBOSE)
#define OEM_PROV_DBG_PRINTF(level, ...)                                        \
	do {                                                                   \
		if (OEM_PROV_DBG_LEVEL_##level <= VERBOSE_LEVEL)               \
			printf(__VA_ARGS__);                                   \
	} while (0)

#else
#define OEM_PROV_DBG_PRINTF(level, ...)
#endif /* ENABLE_VERBOSE */

#define OEM_PROV_PRINTF(...)                                                   \
		printf(__VA_ARGS__)

#endif /* __OEM_PROV_DEBUG_INFO_H__ */
