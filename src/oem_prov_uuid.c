// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 NXP
 */

#include <smw_device.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

#define OEM_PROV_MAX_UUID_LEN 16U

int oem_prov_get_uuid(void)
{
	int res;
	int status = OEM_PROV_STATUS_OK;
	enum smw_status_code smw_status;
	unsigned char buffer[OEM_PROV_MAX_UUID_LEN];

	struct smw_device_uuid_args smw_args = { 0 };

	smw_args.subsystem_name = SMW_SUBSYSTEM_NAME_ELE;
	smw_args.uuid_length = OEM_PROV_MAX_UUID_LEN;
	smw_args.uuid = buffer;

	smw_status = smw_device_get_uuid(&smw_args);
	if (smw_status != SMW_STATUS_OK) {
		status = OEM_PROV_STATUS_SMW_ERROR;
		OEM_PROV_DBG_PRINTF(ERROR, "UUID reading error %d\n",
				    smw_status);
		goto exit;
	}

	OEM_PROV_PRINTF("UUID is 0x");
	for (int i = 0; i < smw_args.uuid_length; i++)
		OEM_PROV_PRINTF("%02X", buffer[i]);

	OEM_PROV_PRINTF("\n");

exit:
	return status;
}
