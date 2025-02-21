// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright 2024-2025 NXP
 */

#include <stdlib.h>
#include "oem_prov_list.h"

static struct node *create_node(void *data)
{
	struct node *node = NULL;

	node = (struct node *)malloc(sizeof(struct node));
	if (node) {
		node->prev = NULL;
		node->next = NULL;
		node->data = data;
	}

	return node;
}

static void destroy_node(struct node *node)
{
	if (node->data)
		free(node->data);
	free(node);
}

void oem_prov_list_init(struct oem_prov_list *list)
{
	list->first = NULL;
	list->last = NULL;
}

void oem_prov_list_destroy(struct oem_prov_list *list)
{
	struct node *next = NULL, *node = NULL;

	node = list->first;
	while (node) {
		next = node->next;
		destroy_node(node);
		node = next;
	}

	oem_prov_list_init(list);
}

int oem_prov_list_insert_last(struct oem_prov_list *list, void *data)
{
	struct node *node = NULL;

	node = create_node(data);
	if (!node)
		return 1;

	if (list->last) {
		list->last->next = node;
		node->prev = list->last;
		list->last = node;
	} else {
		if (list->first)
			return 1;
		list->first = node;
		list->last = node;
	}

	return 0;
}
