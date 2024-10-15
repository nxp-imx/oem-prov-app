// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2023-2024 NXP
 */

#include <nxp_iot_agent.h>
#include <nxp_iot_agent_keystore_psa.h>
#include <nxp_iot_agent_datastore_fs.h>
#include <nxp_iot_agent_datastore_plain.h>
#include <nxp_iot_agent_utils.h>

#include "oem_prov.h"
#include "oem_prov_config.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

#define DATASTORE_EDGELOCK2GO_ID \
	nxp_iot_DatastoreIdentifiers_DATASTORE_EDGELOCK2GO_ID

/**
 * The report was taken as it is from the EdgeLock 2GO Agent code. In the
 * next Agent version this report should be exported as a library function.
 * Until then we keep it as it is although it has a different look and feel
 * from the rest of the code.
 */
const char *
update_status_report_description(nxp_iot_UpdateStatusReport_UpdateStatus status)
{
	switch (status) {
	case nxp_iot_UpdateStatusReport_UpdateStatus_SUCCESS:
		return "SUCCESS";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_ENCODING:
		return "ERR_ENCODING";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_PROTOCOL:
		return "ERR_PROTOCOL";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_MEMORY_READ:
		return "ERR_MEMORY_READ";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_MEMORY_WRITE:
		return "ERR_MEMORY_WRITE";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_SSS_COMMUNICATION:
		return "ERR_SSS_COMMUNICATION";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_SSS_VERSION:
		return "ERR_SSS_VERSION";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_SSS_SECURE_CHANNEL:
		return "ERR_SSS_SECURE_CHANNEL";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_CONFIGURATION:
		return "ERR_CONFIGURATION";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_CONFIGURATION_TOO_MANY_DATASTORES:
		return "ERR_CONFIGURATION_TOO_MANY_DATASTORES";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_CONFIGURATION_TOO_MANY_KEYSTORES:
		return "ERR_CONFIGURATION_TOO_MANY_KEYSTORES";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_CONFIGURATION_SNI_MISSING:
		return "ERR_CONFIGURATION_SNI_MISSING";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_CONFIGURATION_SNI_INVALID:
		return "ERR_CONFIGURATION_SNI_INVALID";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_CONNECTION_QUOTA_EXCEEDED:
		return "ERR_CONNECTION_QUOTA_EXCEEDED";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_DEVICE_NOT_WHITELISTED:
		return "ERR_DEVICE_NOT_WHITELISTED";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_UPDATE_FAILED:
		return "ERR_UPDATE_FAILED";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_INTERNAL:
		return "ERR_INTERNAL";
	case nxp_iot_UpdateStatusReport_UpdateStatus_ERR_TIMEOUT:
		return "ERR_TIMEOUT";
	default:
		return "UNKNOWN";
	}
}

const char *
claim_status_description(nxp_iot_AgentClaimStatus_ClaimStatus status)
{
	switch (status) {
	case nxp_iot_AgentClaimStatus_ClaimStatus_SUCCESS:
		return "SUCCESS";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_NOT_FOUND:
		return "ERR_NOT_FOUND";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_WRONG_PRODUCT_TYPE:
		return "ERR_WRONG_PRODUCT_TYPE";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_REVOKED:
		return "ERR_CLAIM_CODE_REVOKED";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_LIMIT_REACHED:
		return "ERR_CLAIM_CODE_LIMIT_REACHED";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_REUSE_PROHIBITED:
		return "ERR_CLAIM_CODE_REUSE_PROHIBITED";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_READ:
		return "ERR_CLAIM_CODE_READ";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_POLICIES:
		return "ERR_CLAIM_CODE_POLICIES";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_TYPE:
		return "ERR_CLAIM_CODE_TYPE";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_FAILED:
		return "ERR_CLAIM_FAILED";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_CLAIM_CODE_FORMAT:
		return "ERR_CLAIM_CODE_FORMAT";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_TIMEOUT:
		return "ERR_TIMEOUT";
	case nxp_iot_AgentClaimStatus_ClaimStatus_ERR_INTERNAL:
		return "ERR_INTERNAL";
	default:
		return "UNKNOWN";
	}
}

const char *rtp_status_description(nxp_iot_AgentRtpStatus_RtpStatus status)
{
	switch (status) {
	case nxp_iot_AgentRtpStatus_RtpStatus_SUCCESS:
		return "SUCCESS";
	case nxp_iot_AgentRtpStatus_RtpStatus_SUCCESS_NO_CHANGE:
		return "SUCCESS_NO_CHANGE";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_RTP_FAILED:
		return "ERR_RTP_FAILED";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_OBJECT_ATTRIBUTES_READ_FAILED:
		return "ERR_OBJECT_ATTRIBUTES_READ_FAILED";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_OBJECT_DELETE_FAILED:
		return "ERR_OBJECT_DELETE_FAILED";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_OBJECT_WRITE_FAILED:
		return "ERR_OBJECT_WRITE_FAILED";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_DEFECTIVE:
		return "ERR_DEFECTIVE";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_CURVE_INSTALLATION_FAILED:
		return "ERR_CURVE_INSTALLATION_FAILED";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_TIMEOUT:
		return "ERR_TIMEOUT";
	case nxp_iot_AgentRtpStatus_RtpStatus_ERR_INTERNAL:
		return "ERR_INTERNAL";
	default:
		return "UNKNOWN";
	}
}

void print_status_report(const nxp_iot_UpdateStatusReport *status_report)
{
	OEM_PROV_PRINTF("Update status report:\n");
	OEM_PROV_PRINTF(
		"  The device update %s (0x%04x: %s)\n",
		(status_report->status ==
				 nxp_iot_UpdateStatusReport_UpdateStatus_SUCCESS ?
			 "was successful" :
			 "FAILED"),
		status_report->status,
		update_status_report_description(status_report->status));
	OEM_PROV_PRINTF("  The correlation-id for this update is %s.\n",
			status_report->correlationId);

	if (status_report->has_claimStatus) {
		OEM_PROV_PRINTF(
			"  Status for claiming the device: 0x%04x: %s.\n",
			status_report->claimStatus.status,
			claim_status_description(
				status_report->claimStatus.status));
		for (size_t i = 0U;
		     i < status_report->claimStatus.details_count; i++) {
			nxp_iot_AgentClaimStatus_DetailedClaimStatus *s =
				&status_report->claimStatus.details[i];
			OEM_PROV_PRINTF(
				"    On endpoint 0x%08x, status for claiming: 0x%04x: %s.\n",
				s->endpointId, s->status,
				claim_status_description(s->status));
		}
	}

	if (status_report->has_rtpStatus) {
		OEM_PROV_PRINTF(
			"  Status for remote trust provisioning: 0x%04x: %s.\n",
			status_report->rtpStatus.status,
			rtp_status_description(
				status_report->rtpStatus.status));
		for (size_t i = 0U; i < status_report->rtpStatus.details_count;
		     i++) {
			nxp_iot_AgentRtpStatus_RtpObjectStatus *s =
				&status_report->rtpStatus.details[i];
			OEM_PROV_PRINTF(
				"    On endpoint 0x%08x, for object 0x%08x, status: 0x%04x: %s.\n",
				s->endpointId, s->objectId, s->status,
				rtp_status_description(s->status));
		}
	}
}

int oem_prov_online(void)
{
	int status = OEM_PROV_STATUS_OK;
	iot_agent_status_t agent_status = IOT_AGENT_FAILURE;
	iot_agent_context_t iot_agent_context = { 0 };
	iot_agent_keystore_t keystore = { 0 };
	iot_agent_datastore_t el2go_data = { 0 };
	nxp_iot_UpdateStatusReport status_report =
		nxp_iot_UpdateStatusReport_init_default;

	OEM_PROV_DBG_PRINTF(VERBOSE, "IOT agent init\n");
	agent_status = iot_agent_init(&iot_agent_context);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "keystore psa init\n");
	agent_status =
		iot_agent_keystore_psa_init(&keystore, EDGELOCK2GO_KEYSTORE_ID);
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

	OEM_PROV_DBG_PRINTF(VERBOSE, "agent datastore plain init\n");
	agent_status = iot_agent_datastore_plain_init(&el2go_data,
						      DATASTORE_EDGELOCK2GO_ID);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	oem_config_set_host_and_port();
	agent_status = iot_agent_utils_configure_edgelock2go_datastore(&keystore,
								       &el2go_data,
								       0, NULL);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "Set el2go datastore\n");
	agent_status = iot_agent_set_edgelock2go_datastore(&iot_agent_context,
							   &el2go_data);
	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	OEM_PROV_DBG_PRINTF(VERBOSE, "Checking for new secure objects\n");
	agent_status = iot_agent_update_device_configuration(&iot_agent_context,
							     &status_report);

	if (status_report.has_status)
		print_status_report(&status_report);

	if (agent_status != IOT_AGENT_SUCCESS) {
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

	if (!iot_agent_is_service_configuration_data_valid(&iot_agent_context)) {
		OEM_PROV_DBG_PRINTF(ERROR, "Invalid configurations detected\n");
		status = OEM_PROV_STATUS_EL2GO_AGENT_ERROR;
		goto exit;
	}

exit:
	iot_agent_keystore_close_session(&keystore);
	iot_agent_free_update_status_report(&status_report);
	iot_agent_datastore_free(&el2go_data);
	iot_agent_keystore_free(&keystore);

	return agent_status;
}
