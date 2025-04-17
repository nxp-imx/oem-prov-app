// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */
#include <stddef.h>
#include <stdlib.h>

#include "oem_prov_internal.h"
#include "oem_prov_debug_info.h"

static struct oem_prov_os_ctx os_ctx = { 0, NULL, NULL };

inline struct oem_prov_os_ctx *oem_prov_get_os_ctx(void)
{
	return &os_ctx;
}

void oem_prov_set_host_and_port(void)
{
	OEM_PROV_DBG_ASSERT(os_ctx.oem_config && os_ctx.oem_config->online);
	setenv("EDGELOCK2GO_PORT", os_ctx.oem_config->online->port, 1);
	setenv("EDGELOCK2GO_HOSTNAME", os_ctx.oem_config->online->hostname, 1);
}
