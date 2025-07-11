// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */

#include <string.h>
#include <getopt.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov.h"
#include "oem_prov_version.h"
#include "oem_prov_common.h"

#define MAX_FILE_SIZE_NAME 256
#define MAX_PARAM_OPTION 48

static void usage(const char *prg)
{
	OEM_PROV_PRINTF("Usage: %s [OPTION]...\n", prg);
	OEM_PROV_PRINTF("OEM provisioning application\n");
	OEM_PROV_PRINTF("%-40s", "--online,-o file_name");
	OEM_PROV_PRINTF("%s", "Online device provisioning\n");

	OEM_PROV_PRINTF("%-40s", "--offline,-f file_name");
	OEM_PROV_PRINTF("%s", "Offline device provisioning\n");

	OEM_PROV_PRINTF("%-40s", "--claim-code,-c file_name");
	OEM_PROV_PRINTF("%s", "Injects the claim code from the <file_name>\n");

	OEM_PROV_PRINTF("%-40s", "--uuid,-u");
	OEM_PROV_PRINTF("%s", "Prints the device UUID in hex format\n");

	OEM_PROV_PRINTF("%-40s", "--storage,-s commit");
	OEM_PROV_PRINTF("%s", "Commits the secure storage\n");

	OEM_PROV_PRINTF("%-40s", "--life-cycle,-l closed/closed-locked");
	OEM_PROV_PRINTF("%s",
		        "Forwards the device lifecycle to closed/closed-locked.\n");

	OEM_PROV_PRINTF("%-40s", "--version,-v");
	OEM_PROV_PRINTF("%s", "Version information\n");
}

static inline int validate_string(const char *string_value, size_t max_size)
{
	if (strlen(string_value) >= max_size) {
		OEM_PROV_PRINTF("The option is too long\n");
		return OEM_PROV_STATUS_INVALID_STRING;
	}
	return OEM_PROV_STATUS_OK;
}

#define LC_STR_CLOSED "closed"
#define LC_STR_CLOSED_LOCKED "closed-locked"

static int validate_and_convert_life_cycle(const char *lifecycle)
{
	if (!strcmp(lifecycle, LC_STR_CLOSED))
		return OEM_PROV_LC_CLOSED;
	if (!strcmp(lifecycle, LC_STR_CLOSED_LOCKED))
		return OEM_PROV_LC_CLOSED_LOCKED;

	return OEM_PROV_LC_NONE;
}

#define STORAGE_COMMIT "commit"
static int validate_and_convert_storage(const char *storage)
{
	if (!strcmp(storage, STORAGE_COMMIT))
		return OEM_PROV_S_COMMIT;

	return OEM_PROV_S_NONE;
}
int main(int argc, char *argv[])
{
	unsigned int online = 0;
	unsigned int offline = 0;
	unsigned int close = 0;
	unsigned int claim_code = 0;
	unsigned int uuid = 0;
	unsigned int commit_storage = 0;
	int c = 0;
	char config_file_name[MAX_FILE_SIZE_NAME] = { 0 };
	char cc_file_name[MAX_FILE_SIZE_NAME] = { 0 };
	char *close_option = NULL;
	char *storage_option = NULL;
	int status = OEM_PROV_STATUS_OK;
	int status_main = 1;

	while (1) {
		static struct option long_options[] = {
			{ "online", required_argument, 0, 'o' },
			{ "offline", required_argument, 0, 'f' },
			{ "life-cycle", required_argument, 0, 'l' },
			{ "storage", required_argument, 0, 's' },
			{ "claim-code", required_argument, 0, 'c' },
			{ "uuid", no_argument, 0, 'u' },
			{ "version", no_argument, 0, 'v' },
			{ "help", no_argument, 0, 'h' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;

		if (argc <= 1)
			usage(argv[0]);

		c = getopt_long(argc, argv, "f:o:c:huvs:l:", long_options,
				&option_index);

		if (c == -1) {
			if (optind < argc) {
				/* at least one non-option argument exists */
				usage(argv[0]);
			}
			break;
		}

		switch (c) {
		case 'o':
			if (online) {
				usage(argv[0]);
				goto exit;
			}
			online = 1;
			status = validate_string(optarg, MAX_FILE_SIZE_NAME);
			if (status != OEM_PROV_STATUS_OK) {
				usage(argv[0]);
				goto exit;
			}
			strcpy(config_file_name, optarg);
			break;
		case 'f':
			if (offline) {
				usage(argv[0]);
				goto exit;
			}
			offline = 1;
			status = validate_string(optarg, MAX_FILE_SIZE_NAME);
			if (status != OEM_PROV_STATUS_OK) {
				usage(argv[0]);
				goto exit;
			}
			strcpy(config_file_name, optarg);
			break;
		case 'l':
			if (close) {
				usage(argv[0]);
				goto exit;
			}
			close = 1;
			if (validate_string(optarg, MAX_PARAM_OPTION)) {
				usage(argv[0]);
				goto exit;
			}
			close_option = malloc(strlen(optarg) + 1);
			if (!close_option) {
				OEM_PROV_DBG_PRINTF(ERROR,
						    "Allocation error\n");
				goto exit;
			}
			strcpy(close_option, optarg);
			break;
		case 'c':
			if (claim_code) {
				usage(argv[0]);
				goto exit;
			}
			claim_code = 1;
			status = validate_string(optarg, MAX_FILE_SIZE_NAME);
			if (status != OEM_PROV_STATUS_OK) {
				usage(argv[0]);
				goto exit;
			}
			strcpy(cc_file_name, optarg);
			break;
		case 'u':
			if (uuid) {
				usage(argv[0]);
				goto exit;
			}
			uuid = 1;
			break;
		case 's':
			if (commit_storage) {
				usage(argv[0]);
				goto exit;
			}
			commit_storage = 1;
			if (validate_string(optarg, MAX_PARAM_OPTION)) {
				usage(argv[0]);
				goto exit;
			}
			storage_option = malloc(strlen(optarg) + 1);
			if (!storage_option) {
				OEM_PROV_DBG_PRINTF(ERROR,
						    "Allocation error\n");
				goto exit;
			}
			strcpy(storage_option, optarg);
			break;
		case 'v':
			OEM_PROV_PRINTF("Version: %d.%d\n",
					OEM_PROV_VERSION_MAJOR,
					OEM_PROV_VERSION_MINOR);
			break;
		case '?':
			usage(argv[0]);
			goto exit;
		case 'h':
			usage(argv[0]);
			break;

		default:
			usage(argv[0]);
			goto exit;
		}
	}

	/* Input parameters check */
	if (online && offline) {
		OEM_PROV_PRINTF("Select online or offline, not both!\n");
		usage(argv[0]);
		goto exit;
	}

	status = oem_prov_init_smw();
	if (status != OEM_PROV_STATUS_OK)
		goto exit;

	if (uuid) {
		OEM_PROV_DBG_PRINTF(INFO, "UUID read option\n");
		status = oem_prov_get_uuid();
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	if (claim_code) {
		OEM_PROV_DBG_PRINTF(INFO, "Claim code inject option\n");
		status = oem_prov_inject_claimcode(cc_file_name);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	if (online) {
		OEM_PROV_DBG_PRINTF(INFO, "Online provisioning\n");
		status = oem_prov_online(config_file_name);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	if (offline) {
		OEM_PROV_DBG_PRINTF(INFO, "Offline provisioning\n");
		status = oem_prov_offline(config_file_name);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}
	if (commit_storage) {
		int cs = validate_and_convert_storage(storage_option);
		OEM_PROV_DBG_PRINTF(INFO, "Commit storage option\n");

		if (cs == OEM_PROV_S_NONE) {
			OEM_PROV_PRINTF("Invalid option %s\n", storage_option);
			usage(argv[0]);
			goto exit;
		}
		status = oem_prov_commit_key_storage();
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}
	if (close) {
		int option = validate_and_convert_life_cycle(close_option);

		OEM_PROV_DBG_PRINTF(INFO, "Close device option\n");

		if (option == OEM_PROV_LC_NONE) {
			OEM_PROV_PRINTF("Invalid option %s\n", close_option);
			usage(argv[0]);
			goto exit;
		}
		status = oem_prov_set_lifecycle(option);
		if (status != OEM_PROV_STATUS_OK)
			goto exit;
	}

	status_main = 0;

exit:
	if (!status_main)
		OEM_PROV_DBG_PRINTF(INFO, "Status(%s): SUCCESS\n", argv[0]);
	else
		OEM_PROV_DBG_PRINTF(INFO, "Status(%s): FAILURE\n", argv[0]);
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	if (close_option)
		free(close_option);
	if (storage_option)
		free(storage_option);
	return status_main;
}
