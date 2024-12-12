/* SPDX-License-Identifier: BSD-3-Clause */
/*
 * Copyright 2024-2025 NXP
 */

#ifndef __OEM_PROV_LIST_H__
#define __OEM_PROV_LIST_H__

/* List management */

/**
 * struct node - Linked list node
 * @prev: Previous node
 * @next: Next node
 * @data: Pointer to the data contained in the node
 *
 */
struct node {
	struct node *prev;
	struct node *next;
	void *data;
};

/**
 * struct oem_prov_list - Linked list
 * @first: Pointer to the first node of the list
 * @last: Pointer to the last node of the list
 *
 */
struct oem_prov_list {
	struct node *first;
	struct node *last;
};

/**
 * oem_prov_list_init() - Initialize a linked list.
 * @list: Pointer to a linked list.
 *
 * This function initializes a linked list.
 *
 * Return:
 * none.
 */
void oem_prov_list_init(struct oem_prov_list *list);

/**
 * oem_prov_list_destroy() - Destroy a linked list.
 * @list: Pointer to a linked list.
 *
 * This function destroys a linked list.
 * All modes are destroyed.
 * All memory dynamically allocated is freed.
 *
 * Return:
 * none.
 */
void oem_prov_list_destroy(struct oem_prov_list *list);

/**
 * oem_prov_list_insert_last() - Append data to the linked list.
 * @list: Pointer to a linked list.
 * @data: Pointer to the data to be stored.
 *
 * This function creates a node containing the data, and links
 * the node after the last node of the list.
 * The list takes ownership of data and will call free() on it.
 *
 * Return:
 * * 0	- the operation is successful.
 * * 1	- the operation has failed.
 */
int oem_prov_list_insert_last(struct oem_prov_list *list, void *data);

#endif /* __OEM_PROV_LIST_H__ */
