// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2023-2024 NXP
 */

#include <string.h>

#include <smw_osal.h>

#include "oem_prov.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

int oem_prov_init_smw(void)
{
	int res;
	int status = OEM_PROV_STATUS_OK;

	res = smw_osal_lib_init();
	if (res != SMW_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR,
				    "SMW library initialization failed %d\n",
				    res);
		status = OEM_PROV_STATUS_SMW_ERROR;
	}

	return status;
}
