// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2023-2025 NXP
 */
#include "oem_prov.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_common.h"

int oem_prov_indirect(const char *config_filename)
{
	int status = OEM_PROV_STATUS_OK;
	int close_option = 0;

	OEM_PROV_DBG_PRINTF(ERROR, "Indirect option is not yet supported\n");

	status = oem_prov_load_config(config_filename, OEM_PROV_INDIRECT);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid configuration file!\n");
		return status;
	}

	status = oem_prov_parse_objects();
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	/* check if the key storage should be committed */
	if (oem_prov_get_commit_storage(OEM_PROV_INDIRECT)) {
		status = oem_prov_commit_key_storage();
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	/* check if the device needs to be closed */
	close_option = oem_prov_get_lc_option(OEM_PROV_INDIRECT);
	if (close_option)
		status = oem_prov_set_lifecycle(close_option);

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	oem_prov_unload_config();
	return status;
}
