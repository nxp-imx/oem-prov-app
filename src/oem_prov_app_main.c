// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 NXP
 */

#include <string.h>
#include <getopt.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"

#define MAX_FILE_SIZE_NAME 256

static void usage(const char *prg)
{
	OEM_PROV_PRINTF("Usage: %s [OPTION]...\n", prg);
	OEM_PROV_PRINTF("OEM provisioning application\n");
	OEM_PROV_PRINTF("%-30s", "--online,-o file_name");
	OEM_PROV_PRINTF("%s", "Runs the application for the online mode\n");

	OEM_PROV_PRINTF("%-30s", "--indirect,-i file_name");
	OEM_PROV_PRINTF("%s", "Runs the application for the indirect mode\n");

	OEM_PROV_PRINTF("%-30s", "--claim-code,-C file_name");
	OEM_PROV_PRINTF("%s", "Injects the claim code from the <file_name>\n");

	OEM_PROV_PRINTF("%-30s", "--uuid,-u");
	OEM_PROV_PRINTF("%s", "Prints the device UUID in hex format\n");

	OEM_PROV_PRINTF("%-30s", "--close,-c");
	OEM_PROV_PRINTF("%s", "Closes the device\n");
}

static inline int validate_filename(const char *filename)
{
	if (strlen(filename) > MAX_FILE_SIZE_NAME) {
		OEM_PROV_PRINTF("This is a really long name for the file %s.\n",
				filename);
		return OEM_PROV_STATUS_FILENAME_TOO_LONG;
	}

	return OEM_PROV_STATUS_OK;
}

int main(int argc, char *argv[])
{
	struct oem_prov_config *oem_config;
	unsigned int online = 0;
	unsigned int indirect = 0;
	unsigned int close = 0;
	unsigned int claim_code = 0;
	unsigned int uuid = 0;
	int c;
	char config_file_name[MAX_FILE_SIZE_NAME];
	char cc_file_name[MAX_FILE_SIZE_NAME];
	int status = OEM_PROV_STATUS_OK;

	while (1) {
		static struct option long_options[] = {
			{ "online", required_argument, 0, 'o' },
			{ "indirect", required_argument, 0, 'i' },
			{ "close", no_argument, 0, 'c' },
			{ "claim-code", required_argument, 0, 'C' },
			{ "uuid", no_argument, 0, 'u' },
			{ "help", no_argument, 0, 'h' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;

		if (argc <= 1)
			usage(argv[0]);

		c = getopt_long(argc, argv, "i:o:cC:hu", long_options,
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
			online = 1;
			if (validate_filename(optarg) != OEM_PROV_STATUS_OK) {
				usage(argv[0]);
				goto exit;
			}
			strcpy(config_file_name, optarg);
			break;
		case 'i':
			indirect = 1;
			if (validate_filename(optarg) != OEM_PROV_STATUS_OK) {
				usage(argv[0]);
				goto exit;
			}
			strcpy(config_file_name, optarg);
			break;
		case 'c':
			close = 1;
			break;
		case 'C':
			claim_code = 1;
			if (validate_filename(optarg) != OEM_PROV_STATUS_OK) {
				usage(argv[0]);
				goto exit;
			}
			strcpy(cc_file_name, optarg);
			break;
		case 'u':
			uuid = 1;
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
	if (online && indirect) {
		OEM_PROV_DBG_PRINTF(ERROR,
				    "Select online or indirect, not both!\n");
		goto exit;
	}

	if (uuid) {
		OEM_PROV_DBG_PRINTF(INFO, "Selecting UUID inject option\n");
		OEM_PROV_DBG_PRINTF(
			ERROR, "UUID inject option is not yet supported\n");
	}

	if (claim_code) {
		OEM_PROV_DBG_PRINTF(INFO,
				    "Selecting claim code inject option\n");
		OEM_PROV_DBG_PRINTF(ERROR,
				    "Claim code option is not yet supported\n");
	}

	if (online) {
		OEM_PROV_DBG_PRINTF(INFO, "Selecting online provisioning\n");
		OEM_PROV_DBG_PRINTF(ERROR,
				    "Online option is not yet supported\n");
	}

	if (indirect) {
		OEM_PROV_DBG_PRINTF(INFO, "Selecting indirect provisioning\n");
		OEM_PROV_DBG_PRINTF(ERROR,
				    "Indirect option is not yet supported\n");
	}

	if (close) {
		OEM_PROV_DBG_PRINTF(INFO, "Selecting close device option\n");
		OEM_PROV_DBG_PRINTF(ERROR,
				    "Close device is not yet supported\n");
	}

	return 0;

exit:
	return 1;
}
