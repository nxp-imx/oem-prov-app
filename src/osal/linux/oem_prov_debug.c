// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025 NXP
 */
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#include "oem_prov_debug_info.h"
#include "oem_prov_arithmetic_ops.h"

static unsigned int current_log_level = VERBOSE_LEVEL;

void oem_prov_dbg_set_level(char *level_ptr)
{
	char *endptr = NULL;
	unsigned long level = 0;

	level = strtoul(level_ptr, &endptr, 0);
	/* no digits found or invalid base */
	if (!level && endptr == level_ptr)
		return;

	/* extra characters after number */
	if (*endptr != '\0')
		return;

	if (level > OEM_PROV_DBG_LEVEL_MAX)
		level = OEM_PROV_DBG_LEVEL_MAX;

	if (SET_OVERFLOW(level, current_log_level))
		current_log_level = OEM_PROV_DBG_LEVEL_NONE;
}

void oem_prov_dbg_printf(unsigned int level, const char *fmt, ...)
{
	va_list args;

	if (level > current_log_level)
		return;

	va_start(args, fmt);
	(void)vfprintf(stdout, fmt, args);
	va_end(args);
	(void)fflush(stdout);
}
