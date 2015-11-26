#include "stdafx.h"

#include "constants.h"

HANDLE file_reader_run(pipe_in* pipe_in, node* n) {
	file_rw_data* data = (file_rw_data*)malloc(sizeof(file_rw_data));
	data->n = n;
	data->in = pipe_in;
	data->append = 0;

	HANDLE thread = CreateThread(NULL, 0, file_reader_func, (void*)(data), 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

HANDLE file_writter_run(pipe_out* pipe_out, node* n, int append) {
	file_rw_data* data = (file_rw_data*)malloc(sizeof(file_rw_data));
	data->n = n;
	data->out = pipe_out;
	data->append = append;

	HANDLE thread = CreateThread(NULL, 0, file_writer_func, (void*)(data), 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

DWORD WINAPI file_reader_func(void* data) {
	file_rw_data* d = (file_rw_data*)(data);

	node* node = d->n;
	pipe_in* pipe_in = d->in;
	int size, i;

	size = node->content_len;
	if (node->content == NULL) {
		pipe_close_in(pipe_in);
	}
	else if (size <= 0){
		pipe_close_in(pipe_in);
	}
	else {
		for (i = 0; i < size; i++) {
			pipe_write(pipe_in, node->content[i]);
		}
		pipe_close_in(pipe_in);
	}

	node_unlock(node);
	free(data);
	return 0;
}

DWORD WINAPI file_writer_func(void* data) {
	file_rw_data* d = (file_rw_data*)(data);
	node* node = d->n;
	pipe_out* pipe_out = d->out;
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
		c = pipe_read(pipe_out);
		if (c == -1) {
			break;
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