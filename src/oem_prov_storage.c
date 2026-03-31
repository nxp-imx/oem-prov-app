// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025-2026 NXP
 */

#include <smw_keymgr.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_arithmetic_ops.h"

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
	static const char *const warnings[] = {
		"WARNING: You are about to commit the non-volatile key storage.",
		"This operation is IRREVERSIBLE and will:",
		"  - Push the active key storage to physical memory",
		"  - Increment the hardware anti-rollback counter"
	};

	status = oem_prov_confirm_operation(warnings, ARRAY_SIZE(warnings));
	if (status != OEM_PROV_STATUS_OK)
		return status;

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
