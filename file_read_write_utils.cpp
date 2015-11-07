#include "stdafx.h"

#include "constants.h"

void get_path(node* node, char* path, int* position) {
	if (node->parent != NULL) {
		get_path(node->parent, path, position);
	}
	strncpy_s(path + (sizeof(char) * (*position)), 200 - (*position), node->name, node->name_len);
	path[*position + node->name_len] = '/';
	path[*position + node->name_len + 1] = '\0';
	(*position) += node->name_len + 1;
}

node* get_node_by_name(node* dir, char* name) {
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

node* get_node_by_relative_path(node* dir, char* path) {
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
			//printf("comparing: %s and %s\n", immediate_name, listDir[i]->name);

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


HANDLE file_reader_run(pipe* pipe, node* n) {
	file_rw_data* data = (file_rw_data*)malloc(sizeof(file_rw_data));
	data->n = n;
	data->p = pipe;
	data->append = 0;

	HANDLE thread = CreateThread(NULL, 0, file_reader_func, (void*)(data), 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

HANDLE file_writter_run(pipe* pipe, node* n, int append) {
	file_rw_data* data = (file_rw_data*)malloc(sizeof(file_rw_data));
	data->n = n;
	data->p = pipe;
	data->append = append;

	HANDLE thread = CreateThread(NULL, 0, file_writer_func, (void*)(data), 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

DWORD WINAPI file_reader_func(void* data) {
	file_rw_data* d = (file_rw_data*)(data);

	node* node = d->n;
	pipe* pipe = d->p;
	int size, i;

	size = node->content_len;
	if (node->content == NULL) {
		pipe_write(pipe, -1);
	}
	else if (size <= 0){
		pipe_write(pipe, -1);
	}
	else {
		for (i = 0; i < size; i++) {
			pipe_write(pipe, node->content[i]);
		}
		pipe_write(pipe, -1);
	}

	node_unlock(node);
	free(data);
	return 0;
}

DWORD WINAPI file_writer_func(void* data) {
	file_rw_data* d = (file_rw_data*)(data);
	node* node = d->n;
	pipe* pipe = d->p;
	int c, pos, max;
	char* cont;

	if (d->append) {
		if (node->content == NULL) {
			node->content = (char*)malloc(sizeof(char) * 100);
			cont = node->content;
			max = 100;
			pos = 0;
			node->content_len = 0;
		}
		else {
			cont = node->content;
			pos = node->content_len;
			max = pos;
		}
	}
	else {
		if (node->content != NULL) free(node->content);
		max = 100;
		pos = 0;
		cont = (char*)malloc(sizeof(char) * max);
	}

	while (true) {
		c = pipe_read(pipe);
		if (c == -1) {
			if (pipe->auto_close) {
				// TODO destroy pipe
				break;
			}
			else {
				//printf("no autoclose - ignoring -1\n");
			}
		}
		else {
			if (pos < (max-1)) {
				cont[pos] = c;
				pos++;
			}
			else {
				max += 100;
				char* newCont = (char*)malloc(sizeof(char) * max);
				memcpy(newCont, cont, sizeof(char) * (max - 100));
				free(cont);
				cont = newCont;

				cont[pos] = c;
				pos++;

			}
		}
	}
	cont[pos] = -1;
	node->content = cont;
	node->content_len = pos;

	node_unlock(node);
	free(data);
	return 0;
}