// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025 NXP
 */

#include <string.h>
#include <smw_device.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_common.h"

static int convert_option(enum oem_prov_lc_options option)
{
	if (option == OEM_PROV_LC_CLOSED)
		return SMW_LIFECYCLE_NAME_CLOSED;
	if (option == OEM_PROV_LC_CLOSED_LOCKED)
		return SMW_LIFECYCLE_NAME_CLOSED_LOCKED;

	return SMW_LIFECYCLE_NAME_CURRENT;
}

int oem_prov_set_lifecycle(enum oem_prov_lc_options option)
{
	int status = OEM_PROV_STATUS_OK;
	int close_option = 0;
	struct smw_device_lifecycle_args smw_args = { 0 };
	int res = SMW_STATUS_OK;

	close_option = convert_option(option);
	if (close_option == SMW_LIFECYCLE_NAME_CURRENT) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid option %d\n", option);
		return OEM_PROV_STATUS_INVALID_OPTION;
	}

/*
 * If the OEM_PROV_DRY_RUN is set, do not call the smw function. This is used for testing
 * the command line and configuration file options. It is not feasible to close a device
 * each time an interface test is run.
 */
#if defined(OEM_PROV_DRY_RUN)
	OEM_PROV_PRINTF("Lifecycle: Close option is %d\n", option);
	OEM_PROV_PRINTF("Lifecycle: Close option translates to %d\n",
			close_option);
#else
	smw_args.subsystem_name = SMW_SUBSYSTEM_NAME_ELE;
	smw_args.lifecycle_name = close_option;

	res = smw_device_set_lifecycle(&smw_args);
	if (res != SMW_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Set device lifecycle failed: %d\n",
				    res);
		status = OEM_PROV_STATUS_SMW_ERROR;
	}
#endif
	return status;
}
