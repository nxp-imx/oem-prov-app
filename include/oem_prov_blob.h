/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2025 NXP
 */

#ifndef __OEM_PROV_BLOB_H__
#define __OEM_PROV_BLOB_H__

#include <stddef.h>
#include "oem_prov_list.h"
#include "psa/crypto.h"

#define SRKH_KEY_ID 0x7FFF817AU
#define EL2GO_PROV_OEM_KEY_ID 0x7FFF8181U

/**
 * oem_prov_import_all_blobs() - Imports all blobs not already imported into ELE
 *
 * This function goes through the list of metadata and checks each one if each
 * individual asset has already been imported. If it has not already been imported,
 * imports the asset into ELE.
 *
 * @stream: Pointer to the stream (file or memory buffer) to read from.
 * @metadata_list: The head of the linked list where the metadata of the security
 * assets are stored.
 *
 * Return:
 * error code
 */
int oem_prov_import_all_blobs(void *stream, struct oem_prov_list metadata_list);

/**
 * oem_prov_import_blob_by_id() - Imports a security asset with a given id into ELE
 *
 * This function looks for the id of an asset in the metadata list and, if it finds it,
 * reads the entire asset from the stream from the offset specified by the metadata and
 * then imports it into ELE.
 *
 * @stream: Pointer to the stream (file or memory buffer) to read from.
 * @metadata_list: The head of the linked list where the metadata of the security
 * assets are stored.
 * @id: The id of the security asset to be imported.
 * @found: Indicates if the specified blob ID was found
 *
 * Return:
 * error code
 */
int oem_prov_import_blob_by_id(void *stream, struct oem_prov_list metadata_list,
			       psa_key_id_t id, unsigned int *found);

/**
 * oem_prov_extract_blobs_metadata() - Extracts security assets metadata
 *
 * This function does a first parsing of the stream containing the security assets and
 * extracts the metadata of each security asset in a node that is added later in a
 * linked list.
 *
 * @stream: Pointer to the stream (file or memory buffer) to read from.
 * @metadata_list: The address of the head of the linked list where the metadata of the
 * security assets will be stored. This is an output parameter, this function sets it.
 *
 * Return:
 * error code
 */
int oem_prov_extract_blobs_metadata(void *stream,
				    struct oem_prov_list *metadata_list);

/**
 * oem_prov_import_blob() - Imports a security asset into ELE
 *
 * This function calls functions from the PSA API to import a security asset into ELE.
 * First it deletes the security asset from ELE to make sure it doesn't already exist,
 * then it imports it.
 *
 * @data: The buffer where the security asset is stored.
 * @attributes: The buffer where the structure containing the attributes of
 * the security asset is stored.
 * @length: The size of the buffer containing the security asset.
 *
 * Return:
 * error code
 */
int oem_prov_import_blob(unsigned char *data, psa_key_attributes_t *attributes,
			 size_t length);

#endif /* __OEM_PROV_BLOB_H__ */
