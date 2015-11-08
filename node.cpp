#include <stdlib.h>

#include "stdafx.h"

node* node_create(char* name, node* parent, int dir) {
	node* ret = (node*)malloc(sizeof node);

	ret->name = name;
	ret->name_len = strnlen_s(name, sizeof (char) * 50);

	ret->content = NULL;
	ret->content_len = 0;
	ret->directory = dir;

	InitializeCriticalSection(&(ret->file_lock));
	ret->locked = 0;

	ret->dirEntries = NULL;
	ret->entries_count = 0;

	if (parent != NULL) {
		node_add_to_dir(parent, ret);
	}
	else {
		ret->parent = NULL;
	}

	return ret;
}

int node_try_lock(node* node) {
	int ret;
	EnterCriticalSection(&(node->file_lock));
	if (node->locked) {
		ret = 0;
	}
	else {
		node->locked = 1;
		ret = 1;
	}
	LeaveCriticalSection(&(node->file_lock));
	return ret;
}

void node_unlock(node* node) {
	EnterCriticalSection(&(node->file_lock));
	node->locked = 0;
	LeaveCriticalSection(&(node->file_lock));
}

int node_set_content(node* node, char* content, int content_len) {

	node->content = content;
	node->content_len = content_len;

	return 0;
}

int node_get_content_len(node* node) {
	return node->content_len;
}

char* node_get_content(node* node) {
	return node->content;
}

int node_add_to_dir(node* dir, node* child) {
	child->parent = dir;

	if (dir->dirEntries == NULL) {
		dir->dirEntries = (node**)malloc(sizeof(node*) * 1);
		dir->dirEntries[0] = child;
		dir->entries_count = 1;
	}
	else {
		dir->entries_count++;
		node** newEntries = (node**)malloc(dir->entries_count * sizeof(node*));
		newEntries[dir->entries_count - 1] = child;
		memcpy((void*)newEntries, (void*)dir->dirEntries, sizeof(node*) * (dir->entries_count - 1));
		free(dir->dirEntries);
		dir->dirEntries = newEntries;
	}

	return 0;
}

int node_remove_from_dir(node* dir, node* child) {
	int i, pos;
	dir->entries_count--;

	if (dir->entries_count == 0) {
		free(dir->dirEntries);
		dir->dirEntries = NULL;
	}
	else {
		node** newEntries = (node**)malloc(dir->entries_count * sizeof(node*));
		pos = 0;
		for (i = 0; i < dir->entries_count + 1; i++) {
			if (dir->dirEntries[i] != child) {
				newEntries[pos] = dir->dirEntries[i];
				pos++;
			}
		}
		free(dir->dirEntries);
		dir->dirEntries = newEntries;
	}

	return 0;
}

node** node_get_entries(node* dir) {
	return dir->dirEntries;
}

int node_get_entries_count(node* dir) {
	return dir->entries_count;
}