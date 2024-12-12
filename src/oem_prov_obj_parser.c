// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */
#include <string.h>

#include "oem_prov_blob.h"
#include "oem_prov_status.h"
#include "oem_prov_debug_info.h"
#include "oem_prov_os.h"
#include "oem_prov_common.h"
#include "oem_prov_list.h"
#include "psa/crypto.h"

#define TAG_MAGIC 0x40U
#define TAG_KEY_ID 0x41U
#define TAG_PERMITTED_ALGORITHM 0x42U
#define TAG_KEY_USAGE_FLAGS 0x43U
#define TAG_KEY_TYPE 0x44U
#define TAG_KEY_BITS 0x45U
#define TAG_KEY_LIFETIME 0x46U
#define TAG_SIGNATURE 0x5EU

#define LENGTH_TAG 0x01U
#define LENGTH_MAGIC 0x0BU

#define MAX_LLENGTH 0x03U
#define LLENGTH_MAGIC 0x01U

#define LAST_N_BITS(X, N) ((X) & ((1UL << (N)) - 1))

#define VALUE_MAGIC "edgelock2go"

/**
 * struct oem_prov_blob_metadata - Metadata of a security asset
 *
 * This structure contains, in addition to the intrinsic details of a security
 * asset, data about its position in the stream containing all the security
 * assets.
 *
 * @attributes: Intrinsic details of a security asset
 * @length: The size of a security asset
 * @offset: The offset where a security asset begins in the stream
 * @imported: The security asset has already been imported into ELE or not
 */
struct oem_prov_blob_metadata {
	psa_key_attributes_t attributes;
	unsigned int length;
	unsigned int offset;
	unsigned char imported;
};

/**
 * read_length() - Reads the length of a field
 *
 * The length is read (1 byte). If the most significant bit of length is 0, so
 * length < 0x80, the size of the length is 1 byte and the length is the one read.
 * If the most significant bit is 1, then the size of the length is the value
 * obtained from the last 7 least significant bits (let's call it N). Then the
 * length is obtained by concatenating the following N bytes. However, the size
 * of the length cannot be 0 bytes and cannot exceed 3 bytes.
 *
 * @stream: The stream to read from. Reading is done from the current position
 * of the cursor.
 * @length: The length of the field. This is an output parameter, this function
 * sets it.
 *
 * Return:
 * If everything goes well, this function returns a positive value representing
 * the size of the length of the field. If not, returns a negative error code.
 */
static int read_length(void *stream, size_t *length)
{
	int llength = 0;
	size_t ret = 0;

	ret = oem_prov_read_data(length, sizeof(unsigned char), 1, stream);
	if (ret != 1)
		return -OEM_PROV_STATUS_INCOMPLETE_DATA;

	if (*length > 0x80) {
		llength = LAST_N_BITS(*length, 7);
		if (!llength || (llength > MAX_LLENGTH))
			return -OEM_PROV_STATUS_INCOMPLETE_DATA;

		unsigned char data[MAX_LLENGTH] = { 0 };

		ret = oem_prov_read_data(data, sizeof(unsigned char), llength,
					 stream);
		if (ret != llength)
			return -OEM_PROV_STATUS_INCOMPLETE_DATA;
		*length = oem_prov_get_uint_be(data, llength);
	}

	return llength + 1;
}

/**
 * read_data() - Reads data from a stream (file or memory buffer)
 *
 * This function allocates memory for a buffer of given size, reads data of that
 * size from a given stream, and then places it in the allocated buffer.
 *
 * @stream: The stream to read from. Reading is done from the current position
 * of the cursor.
 * @data: The address of the buffer to store read data. This is an output parameter,
 * this function sets it.
 * @length: The size of the buffer containing the read data.
 *
 * Return:
 * error code
 */
static int read_data(void *stream, unsigned char **data, size_t length)
{
	size_t ret = 0;

	(*data) = (unsigned char *)malloc(length);
	if (!(*data))
		return OEM_PROV_STATUS_ALLOCATION_ERROR;

	ret = oem_prov_read_data((*data), sizeof(unsigned char), length,
				 stream);
	if (ret != length)
		return OEM_PROV_STATUS_INCOMPLETE_DATA;

	return OEM_PROV_STATUS_OK;
}

/**
 * add_attribute() - Adds a value to a field in the attributes structure
 *
 * The field that is populated is chosen according to the tag.
 *
 * @attributes: The buffer where the structure containing the attributes of the
 * security asset is stored. This is an output parameter, this function sets it.
 * @tag: Indicates which field of the structure will be set.
 * @value: The buffer where the value to which the field indicated by the tag will
 * be set is stored.
 * @length: The size of the value.
 *
 * Return:
 * none
 */
static void add_attribute(psa_key_attributes_t *attributes, unsigned char tag,
			  unsigned char *value, unsigned int length)
{
	switch (tag) {
	case TAG_KEY_ID:
		psa_set_key_id(attributes, oem_prov_get_uint_be(value, length));
		break;
	case TAG_PERMITTED_ALGORITHM:
		psa_set_key_algorithm(attributes,
				      oem_prov_get_uint_be(value, length));
		break;
	case TAG_KEY_USAGE_FLAGS:
		psa_set_key_usage_flags(attributes,
					oem_prov_get_uint_be(value, length));
		break;
	case TAG_KEY_TYPE:
		psa_set_key_type(attributes,
				 (unsigned short)oem_prov_get_uint_be(value,
								      length));
		break;
	case TAG_KEY_BITS:
		psa_set_key_bits(attributes,
				 (size_t)oem_prov_get_uint_be(value, length));
		break;
	case TAG_KEY_LIFETIME:
		psa_set_key_lifetime(attributes,
				     oem_prov_get_uint_be(value, length));
		break;
	default:
		break;
	}
}

/**
 * is_field_magic() - Check if the current field is magic
 *
 * The function is called after discovering a tag corresponding to a magic field
 * (the field that marks the beginning of a new blob). The function reads the length
 * and value of the field and compares them with the expected values ​​corresponding
 * to the magic field.
 *
 * @stream: The stream to read from. Reading is done from the current position
 * of the cursor.
 *
 * Return:
 * error code
 */
static int is_field_magic(void *stream)
{
	int status = OEM_PROV_STATUS_OK;
	size_t field_length = 0;
	int llength = 0;
	unsigned char *value = NULL;

	llength = read_length(stream, &field_length);
	if (llength != LLENGTH_MAGIC || field_length != LENGTH_MAGIC)
		return OEM_PROV_STATUS_INCOMPLETE_DATA;

	status = read_data(stream, &value, field_length);
	if (status != OEM_PROV_STATUS_OK ||
	    strncmp(value, VALUE_MAGIC, LENGTH_MAGIC))
		status = OEM_PROV_STATUS_INCOMPLETE_DATA;

	free(value);
	return status;
}

int oem_prov_extract_blobs_metadata(void *stream,
				    struct oem_prov_list *metadata_list)
{
	int status = OEM_PROV_STATUS_OK;
	unsigned int count = 0;
	unsigned char tag = 0;
	unsigned char *value = NULL;
	size_t field_length = 0, blob_length = 0, ret = 0;
	int llength = 0;
	psa_key_attributes_t attributes = psa_key_attributes_init();
	struct oem_prov_blob_metadata *blob = NULL;
	struct node *next = NULL;

	oem_prov_list_init(metadata_list);

	while (1) {
		ret = oem_prov_read_data(&tag, sizeof(unsigned char),
					 LENGTH_TAG, stream);
		if (ret != 1 || tag == TAG_MAGIC) {
			if (tag == TAG_MAGIC) {
				status = is_field_magic(stream);
				if (status != OEM_PROV_STATUS_OK)
					goto exit;

				OEM_PROV_DBG_PRINTF(DEBUG,
						    "======= BLOB %u =======\n",
						    1 + count);
			}

			if (count > 0) {
				int size =
					sizeof(struct oem_prov_blob_metadata);
				blob = (struct oem_prov_blob_metadata *)malloc(size);

				blob->attributes = attributes;
				blob->length = blob_length;
				blob->offset = oem_prov_get_offset(stream) -
					       blob_length - (ret == 1);
				blob->imported = 0;

				oem_prov_list_insert_last(metadata_list, blob);
				attributes = psa_key_attributes_init();
			}

			if (ret != 1)
				break;

			count++;
			blob_length = LENGTH_TAG + LLENGTH_MAGIC + LENGTH_MAGIC;
			continue;
		} else {
			blob_length++;
		}

		field_length = 0;
		llength = read_length(stream, &field_length);
		if (llength < 0) {
			status = OEM_PROV_STATUS_INCOMPLETE_DATA;
			goto exit;
		}

		blob_length += llength;

		status = read_data(stream, &value, field_length);
		if (status != OEM_PROV_STATUS_OK) {
			free(value);
			goto exit;
		}

		OEM_PROV_DBG_PRINTF(DEBUG,
				    "[tag: %x, length: %lx]\nvalue: ", tag,
				    field_length);
		for (int i = 0; i < field_length; i++)
			OEM_PROV_DBG_PRINTF(DEBUG, "%02x", value[i]);
		OEM_PROV_DBG_PRINTF(DEBUG, "\n\n");

		add_attribute(&attributes, tag, value, field_length);
		free(value);

		blob_length += field_length;
	}

exit:
	OEM_PROV_DBG_PRINTF(VERBOSE, "%s returned %d\n", __func__, status);
	return status;
}

int oem_prov_import_blob_by_id(void *stream, struct oem_prov_list metadata_list,
			       unsigned int id)
{
	int status = OEM_PROV_STATUS_OK;
	struct node *next = NULL;
	struct oem_prov_blob_metadata *blob = NULL;
	size_t ret = 0;

	next = metadata_list.first;
	while (next) {
		blob = (struct oem_prov_blob_metadata *)(next->data);
		if ((blob->attributes.id == id) && !(blob->imported)) {
			unsigned char *data = NULL;

			data = (unsigned char *)malloc(blob->length);
			if (!data)
				return OEM_PROV_STATUS_ALLOCATION_ERROR;

			if (oem_prov_set_offset(stream, blob->offset, 0)) {
				free(data);
				return OEM_PROV_STATUS_INCOMPLETE_DATA;
			}

			ret = oem_prov_read_data(data, sizeof(unsigned char),
						 blob->length, stream);
			if (ret != blob->length) {
				free(data);
				return OEM_PROV_STATUS_INCOMPLETE_DATA;
			}

			status = oem_prov_import_blob(data, &blob->attributes,
						      blob->length);
			if (status == OEM_PROV_STATUS_OK)
				blob->imported = 1;
			free(data);
			return status;
		}
		next = next->next;
	}
	return OEM_PROV_STATUS_OK;
}

int oem_prov_import_all_blobs(void *stream, struct oem_prov_list metadata_list)
{
	struct node *next = NULL;
	size_t ret = 0;

	if (oem_prov_set_offset(stream, 0, 0))
		return OEM_PROV_STATUS_INCOMPLETE_DATA;

	next = metadata_list.first;
	while (next) {
		struct oem_prov_blob_metadata *blob = NULL;
		unsigned char *data = NULL;

		blob = (struct oem_prov_blob_metadata *)(next->data);
		data = (unsigned char *)malloc(blob->length);
		if (!data)
			return OEM_PROV_STATUS_ALLOCATION_ERROR;

		ret = oem_prov_read_data(data, sizeof(unsigned char),
					 blob->length, stream);
		if (ret != blob->length) {
			free(data);
			return OEM_PROV_STATUS_INCOMPLETE_DATA;
		}

		if (!(blob->imported)) {
			oem_prov_import_blob(data, &blob->attributes,
					     blob->length);
			blob->imported = 1;
		}
		free(data);
		next = next->next;
	}
	return OEM_PROV_STATUS_OK;
}
