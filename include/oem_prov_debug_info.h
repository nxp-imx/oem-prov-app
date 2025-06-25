/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */
#ifndef __OEM_PROV_DEBUG_INFO_H__
#define __OEM_PROV_DEBUG_INFO_H__

#include <stdio.h>
#include <stdlib.h>

#include "oem_prov_common.h"

/* Debug levels*/
#define OEM_PROV_DBG_LEVEL_NONE    0 /* No debug information */
#define OEM_PROV_DBG_LEVEL_ERROR   1 /* Important error for the user*/
#define OEM_PROV_DBG_LEVEL_INFO    2 /* First level of debugging information */
#define OEM_PROV_DBG_LEVEL_DEBUG   3 /* Second level of debugging information */
#define OEM_PROV_DBG_LEVEL_VERBOSE 4 /* Maximum level of debugging information */
#define OEM_PROV_DBG_LEVEL_MAX     OEM_PROV_DBG_LEVEL_VERBOSE

#define OEM_PROV_FLUSH fflush

#if defined(ENABLE_VERBOSE)
#define OEM_PROV_DBG_PRINTF(level, ...) \
	oem_prov_dbg_printf(OEM_PROV_DBG_LEVEL_##level, __VA_ARGS__)

#define OEM_PROV_DBG_ASSERT(exp)                                       \
	do {                                                           \
		if ((exp))                                             \
			break;                                         \
		OEM_PROV_DBG_PRINTF(ERROR,                             \
				    "Assertion \"%s\" failed: in file" \
				    "\"%s\":%d\n",                     \
				    #exp, __FILE__, __LINE__);         \
		OEM_PROV_FLUSH(stdout);                                \
		exit(EXIT_FAILURE);                                    \
	} while (0)

#else
#define OEM_PROV_DBG_PRINTF(level, ...)
#define OEM_PROV_DBG_ASSERT(exp)
#endif /* ENABLE_VERBOSE */

#define OEM_PROV_PRINTF(...)                                                   \
		printf(__VA_ARGS__)

#endif /* __OEM_PROV_DEBUG_INFO_H__ */
