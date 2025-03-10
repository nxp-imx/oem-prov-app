// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2023-2025 NXP
 */

#include <nxp_iot_agent.h>
#include <nxp_iot_agent_keystore.h>
#include <nxp_iot_agent_datastore.h>
#include <nxp_iot_agent_utils.h>

#include "oem_prov.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_common.h"

#define DATASTORE_EDGELOCK2GO_ID \
	nxp_iot_DatastoreIdentifiers_DATASTORE_EDGELOCK2GO_ID

const char *el2go_datastore = "edgelock2go_datastore.bin";

int oem_prov_online(const char *config_filename)
{
	int status = OEM_PROV_STATUS_OK;
	iot_agent_status_t agent_status = IOT_AGENT_SUCCESS;
	iot_agent_context_t iot_agent_context = { 0 };
	iot_agent_keystore_t keystore = { 0 };
	iot_agent_datastore_t el2go_data = { 0 };
	iot_agent_platform_context_t iot_agent_platform_context = { 0 };
	nxp_iot_UpdateStatusReport status_report =
		nxp_iot_UpdateStatusReport_init_default;
	int close_option = 0;

	status = oem_prov_load_config(config_filename, OEM_PROV_ONLINE);
	if (status != OEM_PROV_STATUS_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid configuration file!\n");
		return status;
	}

	/* Perform platform related initializations. It will initialize the
	 * SMW library. The library will not be re-initialized if already
	 * initialized.
	 */
	OEM_PROV_DBG_PRINTF(VERBOSE, "IOT agent platform init\n");
	agent_status =
		iot_agent_platform_init(0, NULL, &iot_agent_platform_context);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "IOT agent init\n");
	agent_status = iot_agent_init(&iot_agent_context);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "keystore psa init\n");
	agent_status = iot_agent_keystore_init(&keystore,
					       EDGELOCK2GO_KEYSTORE_ID,
					       &iot_agent_platform_context);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "Register keystore\n");
	agent_status =
		iot_agent_register_keystore(&iot_agent_context, &keystore);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	/* This data store holds connection information about how to connect
	 * to the EdgeLock 2GO server: server URL, port, server certificate, etc.
	 */
	OEM_PROV_DBG_PRINTF(VERBOSE, "agent datastore plain init\n");
	agent_status = iot_agent_datastore_init(&el2go_data,
						DATASTORE_EDGELOCK2GO_ID, el2go_datastore,
						&iot_agent_service_is_configuration_data_valid);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	/* Configure the connection information. The server URL and port are taken
	 * from the configuration file. Currently the server certificate is hardcoded.
	 */
	oem_prov_set_host_and_port();
	agent_status = iot_agent_utils_configure_edgelock2go_datastore(&keystore,
								       &el2go_data,
								       0, NULL);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	/* Register the data store holding the connection information */
	OEM_PROV_DBG_PRINTF(VERBOSE, "Set el2go datastore\n");
	agent_status = iot_agent_set_edgelock2go_datastore(&iot_agent_context,
							   &el2go_data);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	/* Check for new security assets */
	OEM_PROV_DBG_PRINTF(VERBOSE, "Checking for new secure objects\n");
	agent_status = iot_agent_update_device_configuration(&iot_agent_context,
							     &status_report);

	if (status_report.has_status)
		iot_agent_print_status_report(&status_report);

	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	/* check if the key storage should be committed */
	if (oem_prov_get_storage(OEM_PROV_ONLINE)) {
		status = oem_prov_commit_key_storage();
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	/* check if the device needs to be closed */
	close_option = oem_prov_get_lc_option(OEM_PROV_ONLINE);
	if (close_option)
		status = oem_prov_set_lifecycle(close_option);

exit:
	oem_prov_unload_config();
	iot_agent_keystore_close_session(&keystore);
	iot_agent_free_update_status_report(&status_report);
	iot_agent_datastore_free(&el2go_data);
	iot_agent_keystore_free(&keystore);

	return status;
}
