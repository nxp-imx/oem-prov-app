// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024 NXP
 */

#include <stdlib.h>
#include <stdio.h>
#include <cyaml/cyaml.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_config.h"

struct oem_prov_online {
	char *hostname;
	char *port;
	unsigned int close;
};

struct oem_prov_indirect {
	char *partition;
	char *type;
	char *mount_point;
	char *file_name;
	unsigned int close;
};
struct oem_prov_config {
	struct oem_prov_online *online;
	struct oem_prov_indirect *indirect;
};

static const cyaml_schema_field_t online_schema[] = {
	CYAML_FIELD_STRING_PTR("hostname", CYAML_FLAG_POINTER,
			       struct oem_prov_online, hostname, 0,
			       CYAML_UNLIMITED),

	CYAML_FIELD_STRING_PTR("port", CYAML_FLAG_POINTER,
			       struct oem_prov_online, port, 0,
			       CYAML_UNLIMITED),

	CYAML_FIELD_INT("close", CYAML_FLAG_DEFAULT, struct oem_prov_online,
			close),

	CYAML_FIELD_END

};

static const cyaml_schema_field_t indirect_schema[] = {
	CYAML_FIELD_STRING_PTR("partition", CYAML_FLAG_POINTER,
			       struct oem_prov_indirect, partition, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("type", CYAML_FLAG_POINTER,
			       struct oem_prov_indirect, type, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("mount_point", CYAML_FLAG_POINTER,
			       struct oem_prov_indirect, mount_point, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("file_name", CYAML_FLAG_POINTER,
			       struct oem_prov_indirect, file_name, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_INT("close", CYAML_FLAG_DEFAULT, struct oem_prov_indirect,
			close),
	CYAML_FIELD_END

};

#define CYMAL_FLAG_OPTIONAL_POINTER (CYAML_FLAG_POINTER | CYAML_FLAG_OPTIONAL)

static const cyaml_schema_field_t oem_prov_config_fields_schema[] = {

	CYAML_FIELD_MAPPING_PTR("online", CYMAL_FLAG_OPTIONAL_POINTER,
				struct oem_prov_config, online, online_schema),

	CYAML_FIELD_MAPPING_PTR("indirect", CYMAL_FLAG_OPTIONAL_POINTER,
				struct oem_prov_config, indirect,
				indirect_schema),
	CYAML_FIELD_END
};

static const cyaml_schema_value_t oem_prov_config_schema = {
	CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct oem_prov_config,
			    oem_prov_config_fields_schema)
};

static const cyaml_config_t config = {
	.log_fn = cyaml_log,		/* Use the default logging function. */
	.mem_fn = cyaml_mem,		/* Use the default memory allocator. */
	.log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
};

static struct oem_prov_config *oem_config;

/**
 * oem_prov_validate_option() - Validates the configuration file
 *
 * This function validates if the selected command line option has
 * the needed options in the configuration file.
 *
 * Return:
 * error code
 */
static int oem_prov_validate_option(int option)
{
	if (!oem_config)
		return OEM_PROV_STATUS_EMPTY_FILE;

	switch (option) {
	case OEM_PROV_ONLINE:
		if (!oem_config->online)
			return OEM_PROV_STATUS_ONLINE_OPT_MISSING;
		OEM_PROV_DBG_PRINTF(INFO, "Using host: %s[%s]\n",
				    oem_config->online->hostname,
				    oem_config->online->port);
		OEM_PROV_DBG_PRINTF(INFO,
				    "Close the board after provisioning: %d\n",
				    oem_config->online->close);
		break;
	case OEM_PROV_INDIRECT:
		if (!oem_config->indirect)
			return OEM_PROV_STATUS_INDIRECT_OPT_MISSING;
		OEM_PROV_DBG_PRINTF(INFO, "partition: %s\n",
				    oem_config->indirect->partition);
		OEM_PROV_DBG_PRINTF(INFO, "type: %s\n",
				    oem_config->indirect->type);
		OEM_PROV_DBG_PRINTF(INFO, "mount_point: %s\n",
				    oem_config->indirect->mount_point);
		OEM_PROV_DBG_PRINTF(INFO, "file_name: %s\n",
				    oem_config->indirect->file_name);
		OEM_PROV_DBG_PRINTF(INFO, "close: %d\n",
				    oem_config->indirect->close);
		break;
	}
	return OEM_PROV_STATUS_OK;
}

int oem_load_config_file(const char *filename, int option)
{
	cyaml_err_t err;
	int status = OEM_PROV_STATUS_OK;

	OEM_PROV_DBG_PRINTF(VERBOSE, "Loading config file %s\n", filename);

	/* Load input files */
	err = cyaml_load_file(filename, &config, &oem_prov_config_schema,
			      (void **)&oem_config, NULL);

	if (err != CYAML_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "ERROR: %s\n", cyaml_strerror(err));
		return OEM_PROV_STATUS_INVALID_FILE;
	}
	status = oem_prov_validate_option(option);

	return status;
}

void oem_config_set_host_and_port(void)
{
	setenv("EDGELOCK2GO_PORT", oem_config->online->port, 1);
	setenv("EDGELOCK2GO_HOSTNAME", oem_config->online->hostname,
	       1);
}

void oem_unload_config(void)
{
	cyaml_free(&config, &oem_prov_config_schema, oem_config, 0);
}
