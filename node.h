#pragma once
#ifndef NODE_H
#define NODE_H

#include "stdafx.h"

#define FILE_SEPARATOR '\\'

typedef struct node_s {

	char* name;
	int name_len;

	node_s* parent;

	CRITICAL_SECTION file_lock;
	int locked;

	char* content;
	int content_len;

	node_s** dirEntries;
	int entries_count;

} node;

node* node_create(char* name, node* parent);
int node_set_content(node* node, char* content, int content_len);
int node_get_content_len(node* node);
char* node_get_content(node* node);
int node_add_to_dir(node* dir, node* child);
node** node_get_entries(node* dir);
int node_get_entries_count(node* dir);

int node_try_lock(node* node);
void node_unlock(node* node);

#endif