// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2023-2024 NXP
 */
#include "oem_prov.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_common.h"

int oem_prov_indirect(const char *config_filename)
{
	int status = OEM_PROV_STATUS_OK;

	OEM_PROV_DBG_PRINTF(INFO, "Selecting indirect provisioning\n");
	OEM_PROV_DBG_PRINTF(ERROR, "Indirect option is not yet supported\n");

	status = oem_prov_load_config(config_filename, OEM_PROV_INDIRECT);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid configuration file!\n");
		return status;
	}

	status = oem_prov_parse_objects();
	oem_prov_unload_config();

	return status;
}
