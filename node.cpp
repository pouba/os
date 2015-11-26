#include <stdlib.h>

#include "stdafx.h"

node* node_create(char* name, node* parent, int dir) {
	node* ret = (node*)malloc(sizeof node);
	int i;

	for (i = 0; i < strlen(name); i++) {
		if ((name[i] >= 'a') && (name[i] <= 'z')) continue;
		if ((name[i] >= 'A') && (name[i] <= 'Z')) continue;
		if ((name[i] >= '0') && (name[i] <= '9')) continue;
		if (name[i] == ':') continue;

		return NULL;
	}

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

void get_path(node* node, char* path, int* position) {
	if (node->parent != NULL) {
		if (node->parent->parent != NULL) {
			get_path(node->parent, path, position);
		}
	}
	strncpy_s(path + (sizeof(char) * (*position)), 200 - (*position), node->name, node->name_len);
	path[*position + node->name_len] = FILE_SEPARATOR;
	path[*position + node->name_len + 1] = '\0';
	(*position) += node->name_len + 1;
}

node* node_get(char* path, node* root, node* act) {
	node* ret;

	// get by relative path
	ret = node_get_by_relative_path(act, path);
	if (ret != NULL) return ret;

	// get by absolute path
	if (root->parent != NULL) {
		ret = node_get_by_relative_path(root->parent, path);
		if (ret != NULL) return ret;
	}

	// null
	return NULL;
}

node* node_get_by_name(node* dir, char* name) {
	node** listDir = node_get_entries(dir);
	int i = 0;
	int max = node_get_entries_count(dir);
	for (i = 0; i < max; i++) {
		if (strcmp(name, listDir[i]->name) == 0) {
			return listDir[i];
		}
	}

	return NULL;
}

node* node_get_by_relative_path(node* dir, char* path) {
	int i, pos = 0, found = 0;
	char* immediate_name = (char*)malloc(sizeof(char) * MAX_PATH_LEN);
	node* act_node = dir;

	while (1) {
		if (found && (path[pos] == '\0')) return act_node;

		for (i = 0; ; i++) {
			if ((path[pos] == '\0') || (path[pos] == FILE_SEPARATOR)) {
				immediate_name[i] = '\0';
				if (path[pos] == FILE_SEPARATOR) {
					pos++;
				}
				break;
			}
			else {
				immediate_name[i] = path[pos];
				pos++;
			}
		}

		//	printf("imm: %s\n", immediate_name);

		node** listDir = node_get_entries(act_node);
		int max = node_get_entries_count(act_node);
		found = 0;
		for (i = 0; i < max; i++) {
			if (strcmp(immediate_name, listDir[i]->name) == 0) {
				act_node = listDir[i];
				found = 1;
				break;
			}
		}
		if (!found) return NULL;

	}

	return NULL;
}