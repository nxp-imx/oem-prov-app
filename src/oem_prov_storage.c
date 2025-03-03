// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025 NXP
 */

#include <smw_keymgr.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

int oem_prov_commit_key_storage(void)
{
	int status = OEM_PROV_STATUS_OK;

/*
 * If the OEM_PROV_DRY_RUN is set, do not call the smw function. This is used for testing
 * the command line and configuration file options. It is not feasible to commit storage
 * for interface tests
 */
#if defined(OEM_PROV_DRY_RUN)
	OEM_PROV_PRINTF("Commit storage\n");
#else

	struct smw_commit_key_storage_args smw_args = { 0 };
	int res = SMW_STATUS_OK;

	smw_args.subsystem_name = SMW_SUBSYSTEM_NAME_ELE;
	res = smw_commit_key_storage(&smw_args);
	if (res != SMW_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR,
				    "Committing storage failed with error %d\n",
				    res);
		status = OEM_PROV_STATUS_SMW_ERROR;
	}
#endif

	return status;
}
