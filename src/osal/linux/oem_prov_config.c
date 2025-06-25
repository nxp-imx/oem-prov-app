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
#include "oem_prov_arithmetic_ops.h"

#define CYMAL_FLAG_OPTIONAL_POINTER (CYAML_FLAG_POINTER | CYAML_FLAG_OPTIONAL)

static const cyaml_strval_t lc_strings[] = {
	{ "closed", OEM_PROV_LC_CLOSED },
	{ "closed-locked", OEM_PROV_LC_CLOSED_LOCKED },
};

static const cyaml_strval_t provisioning_str[] = {
	{ "individual", OEM_PROV_FLOW_IND },
	{ "product", OEM_PROV_FLOW_PROD },
};

static const cyaml_strval_t cs_strings[] = {
	{ "commit", OEM_PROV_S_COMMIT },
};

static const cyaml_strval_t bool_strings[] = {
	{ "yes", OEM_PROV_TRUE },
	{ "no", OEM_PROV_FALSE }
};

static const cyaml_schema_value_t string_ptr_schema = {
	CYAML_VALUE_STRING(CYAML_FLAG_POINTER, char, 0, CYAML_UNLIMITED),
};

static const cyaml_schema_field_t online_schema[] = {
	CYAML_FIELD_STRING_PTR("hostname", CYAML_FLAG_POINTER,
			       struct oem_prov_online, hostname, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("port", CYAML_FLAG_POINTER,
			       struct oem_prov_online, port, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("server_cert", CYMAL_FLAG_OPTIONAL_POINTER,
			       struct oem_prov_online, server_cert, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_ENUM("lifecycle", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_online, close, lc_strings,
			 CYAML_ARRAY_LEN(lc_strings)),
	CYAML_FIELD_ENUM("storage", CYAML_FLAG_OPTIONAL, struct oem_prov_online,
			 commit_storage, cs_strings,
			 CYAML_ARRAY_LEN(cs_strings)),

	CYAML_FIELD_END

};

static const cyaml_schema_field_t offline_schema[] = {
	CYAML_FIELD_STRING_PTR("partition", CYAML_FLAG_POINTER,
			       struct oem_prov_offline, partition, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("type", CYAML_FLAG_POINTER,
			       struct oem_prov_offline, type, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_STRING_PTR("mount_point", CYAML_FLAG_POINTER,
			       struct oem_prov_offline, mount_point, 0,
			       CYAML_UNLIMITED),
	CYAML_FIELD_SEQUENCE("file_name", CYAML_FLAG_POINTER,
			     struct oem_prov_offline, file_name,
			     &string_ptr_schema, 0, 1),
	CYAML_FIELD_ENUM("delete_assets_file", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_offline, delete_assets, bool_strings,
			 CYAML_ARRAY_LEN(bool_strings)),
	CYAML_FIELD_ENUM("lifecycle", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_offline, close, lc_strings,
			 CYAML_ARRAY_LEN(lc_strings)),
	CYAML_FIELD_ENUM("storage", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_offline, commit_storage, cs_strings,
			 CYAML_ARRAY_LEN(cs_strings)),
	CYAML_FIELD_ENUM("provisioning", CYAML_FLAG_OPTIONAL,
			 struct oem_prov_offline, flow, provisioning_str,
			 CYAML_ARRAY_LEN(provisioning_str)),

	CYAML_FIELD_END

};

static const cyaml_schema_field_t oem_prov_config_fields_schema[] = {

	CYAML_FIELD_MAPPING_PTR("online", CYMAL_FLAG_OPTIONAL_POINTER,
				struct oem_prov_config, online, online_schema),

	CYAML_FIELD_MAPPING_PTR("offline", CYMAL_FLAG_OPTIONAL_POINTER,
				struct oem_prov_config, offline,
				offline_schema),
	CYAML_FIELD_END
};

static const cyaml_schema_value_t oem_prov_config_schema = {
	CYAML_VALUE_MAPPING(CYAML_FLAG_POINTER, struct oem_prov_config,
			    oem_prov_config_fields_schema)
};

/**
 * oem_prov_cyaml_mem() - Custom allocation function
 *
 * The cymal library has a default allocation function. Add a custom allocation
 * function which is initializing the allocated structure. Although cyaml does
 * set default values for optional fields, it is safer to initialize the memory.
 * @ctx: Context passed by the user
 * @ptr: Pointer to realloc, NULL for allocation
 * @size: Allocation size, 0 for free
 * Return:
 * pointer to te allocate memory
 */
void *oem_prov_cyaml_mem(void *ctx, void *ptr, size_t size)
{
	void *ret_ptr = NULL;

	if (size == 0) {
		free(ptr);
		return NULL;
	}

	ret_ptr = realloc(ptr, size);

	if (!ret_ptr)
		return NULL;

	if (!ptr)
		memset(ret_ptr, 0, size);

	return ret_ptr;
}

static const cyaml_config_t config = {
	.log_fn = cyaml_log,              /* Use the default logging function. */
	.mem_fn = oem_prov_cyaml_mem,     /* Use a custom memory allocation */
	.log_level = CYAML_LOG_WARNING,   /* Logging errors and warnings only. */
};

const char *get_str_for_int(int option, const cyaml_strval_t *array, int len)
{
	for (int i = 0; i < len; i++)
		if (array[i].val == option)
			return array[i].str;
	return NULL;
}

static void print_online_info(struct oem_prov_online *config)
{
	OEM_PROV_DBG_PRINTF(INFO, "Online provisioning\n");
	OEM_PROV_DBG_PRINTF(INFO, "\thost: %s[%s]\n", config->hostname,
			    config->port);
	if (config->commit_storage)
		OEM_PROV_DBG_PRINTF(INFO, "\tstorage: %s\n",
				    get_str_for_int(config->commit_storage,
						    cs_strings,
						    ARRAY_SIZE(cs_strings)));
	if (config->close)
		OEM_PROV_DBG_PRINTF(INFO, "\tlifecycle: %s\n",
				    get_str_for_int(config->close, lc_strings,
						    ARRAY_SIZE(lc_strings)));
	if (config->server_cert)
		OEM_PROV_DBG_PRINTF(INFO, "\tserver_cert: %s\n",
				    config->server_cert);
}

static void print_offline_info(struct oem_prov_offline *config)
{
	OEM_PROV_DBG_PRINTF(INFO, "Offline provisioning\n");
	OEM_PROV_DBG_PRINTF(INFO, "\tpartition: %s\n", config->partition);
	OEM_PROV_DBG_PRINTF(INFO, "\ttype: %s\n", config->type);
	OEM_PROV_DBG_PRINTF(INFO, "\tmount_point: %s\n", config->mount_point);

	OEM_PROV_DBG_PRINTF(INFO, "\tfile_name: ");
	for (int i = 0; i < config->file_name_count; i++)
		OEM_PROV_DBG_PRINTF(INFO, "%s ", config->file_name[i]);
	OEM_PROV_DBG_PRINTF(INFO, "\n");

	if (config->delete_assets)
		OEM_PROV_DBG_PRINTF(INFO, "\tdelete_assets_file: %s\n",
				    get_str_for_int(config->delete_assets,
						    bool_strings,
						    ARRAY_SIZE(bool_strings)));
	if (config->commit_storage)
		OEM_PROV_DBG_PRINTF(INFO, "\tstorage: %s\n",
				    get_str_for_int(config->commit_storage,
						    cs_strings,
						    ARRAY_SIZE(cs_strings)));
	if (config->close)
		OEM_PROV_DBG_PRINTF(INFO, "\tlifecycle: %s\n",
				    get_str_for_int(config->close, lc_strings,
						    ARRAY_SIZE(lc_strings)));
	if (config->flow)
		OEM_PROV_DBG_PRINTF(INFO, "\tprovisioning: %s\n",
				    get_str_for_int(config->flow, provisioning_str,
						    ARRAY_SIZE(provisioning_str)));
}

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
	if (!oem_config)
		return OEM_PROV_STATUS_EMPTY_FILE;

	switch (option) {
	case OEM_PROV_ONLINE:
		if (!oem_config->online)
			return OEM_PROV_STATUS_ONLINE_OPT_MISSING;
		print_online_info(oem_config->online);
		break;
	case OEM_PROV_OFFLINE:
		if (!oem_config->offline)
			return OEM_PROV_STATUS_OFFLINE_OPT_MISSING;
		print_offline_info(oem_config->offline);
		break;
	}

	return OEM_PROV_STATUS_OK;
}

int oem_prov_load_config(const char *filename, int option)
{
	cyaml_err_t err = CYAML_OK;
	int status = OEM_PROV_STATUS_OK;
	struct oem_prov_os_ctx *os_ctx = NULL;

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
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();

	if (os_ctx)
		cyaml_free(&config, &oem_prov_config_schema, os_ctx->oem_config,
			   0);
}

int oem_prov_get_lc_option(unsigned int mode)
{
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();

	switch (mode) {
	case OEM_PROV_ONLINE:
		return os_ctx->oem_config->online->close;
	case OEM_PROV_OFFLINE:
		return os_ctx->oem_config->offline->close;
	}

	return 0;
}

int oem_prov_get_prov_type(void)
{
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();
	return os_ctx->oem_config->offline->flow;
}

int oem_prov_get_storage(unsigned int mode)
{
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();

	switch (mode) {
	case OEM_PROV_ONLINE:
		return os_ctx->oem_config->online->commit_storage;
	case OEM_PROV_OFFLINE:
		return os_ctx->oem_config->offline->commit_storage;
	}

	return 0;
}

char *oem_prov_get_server_cert(void)
{
	struct oem_prov_os_ctx *os_ctx = NULL;

	os_ctx = oem_prov_get_os_ctx();

	return os_ctx->oem_config->online->server_cert;
}
