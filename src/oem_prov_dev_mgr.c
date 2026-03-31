// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2025-2026 NXP
 */

#include <string.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_common.h"
#include "oem_prov_lifecycle.h"
#include "oem_prov_os.h"
#include "oem_prov_arithmetic_ops.h"

static const char *lifecycle_to_string(smw_lifecycle_t lifecycle)
{
	switch (lifecycle) {
	case SMW_LIFECYCLE_NAME_OPEN:
		return "OPEN";
	case SMW_LIFECYCLE_NAME_CLOSED:
		return "CLOSED";
	case SMW_LIFECYCLE_NAME_CLOSED_LOCKED:
		return "CLOSED-LOCKED";
	default:
		return "UNKNOWN";
	}
}

static int convert_option(enum oem_prov_lc_options option)
{
	if (option == OEM_PROV_LC_CLOSED)
		return SMW_LIFECYCLE_NAME_CLOSED;
	if (option == OEM_PROV_LC_CLOSED_LOCKED)
		return SMW_LIFECYCLE_NAME_CLOSED_LOCKED;

	return SMW_LIFECYCLE_NAME_CURRENT;
}

int oem_prov_read_lifecycle(smw_lifecycle_t *lifecycle)
{
	int status = OEM_PROV_STATUS_OK;
	struct smw_device_lifecycle_args smw_args = { 0 };
	int res = SMW_STATUS_OK;

	if (!lifecycle) {
		status = OEM_PROV_STATUS_INVALID_POINTER;
		goto exit;
	}

	smw_args.subsystem_name = SMW_SUBSYSTEM_NAME_ELE;

	res = smw_device_get_lifecycle(&smw_args);
	if (res != SMW_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Get device lifecycle failed: %d\n",
				    res);
		status = OEM_PROV_STATUS_SMW_ERROR;
		goto exit;
	}

	*lifecycle = smw_args.lifecycle_name;
exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_get_lifecycle(void)
{
	int status = OEM_PROV_STATUS_OK;
	smw_lifecycle_t lifecycle = SMW_LIFECYCLE_NAME_NONE;

	status = oem_prov_read_lifecycle(&lifecycle);
	if (status != OEM_PROV_STATUS_OK)
		return status;

	OEM_PROV_PRINTF("Device lifecycle: %s\n",
			lifecycle_to_string(lifecycle));
	return status;
}

int oem_prov_set_lifecycle(enum oem_prov_lc_options option)
{
	int status = OEM_PROV_STATUS_OK;
	int close_option = 0;

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
	struct smw_device_lifecycle_args smw_args = { 0 };
	int res = SMW_STATUS_OK;
	static const char *const warnings[] = {
		"WARNING: You are about to forward the device lifecycle.",
		"This operation is IRREVERSIBLE and cannot be undone."
	};

	status = oem_prov_confirm_operation(warnings, ARRAY_SIZE(warnings));
	if (status != OEM_PROV_STATUS_OK)
		return status;

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
