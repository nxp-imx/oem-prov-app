// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */

#include <stdlib.h>
#include <stdio.h>
#include <cyaml/cyaml.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_internal.h"
#include "oem_prov_common.h"

#define CYMAL_FLAG_OPTIONAL_POINTER (CYAML_FLAG_POINTER | CYAML_FLAG_OPTIONAL)

static const cyaml_strval_t lc_strings[] = {
	{ "closed", OEM_PROV_LC_CLOSED },
	{ "closed-locked", OEM_PROV_LC_CLOSED_LOCKED },
};

#define CS_ENABLE 1
static const cyaml_strval_t cs_strings[] = {
	{ "Yes", CS_ENABLE },
	{ "yes", CS_ENABLE },
};

static const cyaml_schema_field_t online_schema[] = {
	CYAML_FIELD_STRING_PTR("hostname", CYAML_FLAG_POINTER,
			       struct oem_prov_online, hostname, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("port", CYAML_FLAG_POINTER,
			       struct oem_prov_online, port, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_ENUM("lifecycle", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_online, close, lc_strings,
			 CYAML_ARRAY_LEN(lc_strings)),
	CYAML_FIELD_ENUM("commit_storage", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_online, commit_storage, cs_strings,
			 CYAML_ARRAY_LEN(cs_strings)),

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
	CYAML_FIELD_ENUM("lifecycle", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_indirect, close, lc_strings,
			 CYAML_ARRAY_LEN(lc_strings)),
	CYAML_FIELD_ENUM("commit_storage", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_indirect, commit_storage, cs_strings,
			 CYAML_ARRAY_LEN(cs_strings)),

	CYAML_FIELD_END

};

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
	.log_fn = cyaml_log,            /* Use the default logging function. */
	.mem_fn = cyaml_mem,            /* Use the default memory allocator. */
	.log_level = CYAML_LOG_WARNING, /* Logging errors and warnings only. */
};

/**
 * oem_prov_validate_option() - Validates the configuration file
 *
 * This function validates if the selected command line option has
 * the needed options in the configuration file.
 *
 * Return:
 * error code
 */
static int oem_prov_validate_option(int option,
				    struct oem_prov_config *oem_config)
{
	int status = OEM_PROV_STATUS_OK;
	int close = 0;
	int commit_storage = 0;

	if (!oem_config)
		return OEM_PROV_STATUS_EMPTY_FILE;

	switch (option) {
	case OEM_PROV_ONLINE:
		if (!oem_config->online)
			return OEM_PROV_STATUS_ONLINE_OPT_MISSING;
		close = oem_config->online->close;
		commit_storage = oem_config->online->commit_storage;

		OEM_PROV_DBG_PRINTF(INFO, "Using host: %s[%s]\n",
				    oem_config->online->hostname,
				    oem_config->online->port);
		break;
	case OEM_PROV_INDIRECT:
		if (!oem_config->indirect)
			return OEM_PROV_STATUS_INDIRECT_OPT_MISSING;
		close = oem_config->indirect->close;
		commit_storage = oem_config->indirect->commit_storage;

		OEM_PROV_DBG_PRINTF(INFO, "partition: %s\n",
				    oem_config->indirect->partition);
		OEM_PROV_DBG_PRINTF(INFO, "type: %s\n",
				    oem_config->indirect->type);
		OEM_PROV_DBG_PRINTF(INFO, "mount_point: %s\n",
				    oem_config->indirect->mount_point);
		OEM_PROV_DBG_PRINTF(INFO, "file_name: %s\n",
				    oem_config->indirect->file_name);
		break;
	}

	if (commit_storage) {
		OEM_PROV_DBG_PRINTF(INFO, "Commit storage: %d\n",
				    commit_storage);
	}

	if (close)
		OEM_PROV_DBG_PRINTF(INFO, "Close: %d\n", close);

	return OEM_PROV_STATUS_OK;
}

int oem_prov_load_config(const char *filename, int option)
{
	cyaml_err_t err;
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_os_ctx *os_ctx;

	os_ctx = oem_prov_get_os_ctx();

	OEM_PROV_DBG_PRINTF(VERBOSE, "Loading config file %s\n", filename);

	OEM_PROV_DBG_ASSERT(os_ctx);
	/* Load input files */
	err = cyaml_load_file(filename, &config, &oem_prov_config_schema,
			      (void **)&os_ctx->oem_config, NULL);

	if (err != CYAML_OK) {
		OEM_PROV_DBG_PRINTF(ERROR, "ERROR: %s\n", cyaml_strerror(err));
		return OEM_PROV_STATUS_INVALID_FILE;
	}
	status = oem_prov_validate_option(option, os_ctx->oem_config);

	return status;
}

void oem_prov_unload_config(void)
{
	struct oem_prov_os_ctx *os_ctx;
	struct oem_prov_config *oem_config;

	os_ctx = oem_prov_get_os_ctx();

	OEM_PROV_DBG_ASSERT(os_ctx);
	cyaml_free(&config, &oem_prov_config_schema, os_ctx->oem_config, 0);
}

int oem_prov_get_lc_option(unsigned int mode)
{
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();

	switch (mode) {
	case OEM_PROV_ONLINE:
		return os_ctx->oem_config->online->close;
	case OEM_PROV_INDIRECT:
		return os_ctx->oem_config->indirect->close;
	}

	return 0;
}

int oem_prov_get_commit_storage(unsigned int mode)
{
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();

	switch (mode) {
	case OEM_PROV_ONLINE:
		return os_ctx->oem_config->online->commit_storage;
	case OEM_PROV_INDIRECT:
		return os_ctx->oem_config->indirect->commit_storage;
	}

	return 0;
}
