// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_internal.h"

int oem_prov_parse_objects(void)
{
	int status = OEM_PROV_STATUS_OK;
	char *assets_file = NULL;

	status = oem_prov_load_assets_file(&assets_file);
	if (status != OEM_PROV_STATUS_OK)
		return status;
	OEM_PROV_DBG_PRINTF(VERBOSE, "Assets file is %s\n", assets_file);

	/* for testing purpose only - to be removed when assets parsing is implemented */
	FILE *fp = fopen(assets_file, "r");

	if (!fp) {
		status = OEM_PROV_STATUS_INVALID_FILE;
		OEM_PROV_DBG_PRINTF(ERROR, "Error opening file %s\n",
				    assets_file);
		goto exit;
	}
	char buffer[256];

	while (fgets(buffer, sizeof(buffer), fp))
		OEM_PROV_PRINTF("%s", buffer);
	fclose(fp);
	/* end testing area*/

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	free(assets_file);
	oem_prov_unload_assets_file();
	return status;
}
