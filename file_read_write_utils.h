#include "stdafx.h"

typedef struct file_rw_data_t {
	pipe* p;
	node* n;
} file_rw_data;

void get_path(node* node, char* path, int* position);
node* get_node_by_name(node* dir, char* name);
node* get_node_by_relative_path(node* dir, char* path);


HANDLE file_reader_run(pipe* pipe, node* n);
HANDLE file_writter_run(pipe* pipe, node* n);

DWORD WINAPI file_writer_func(void* data);
DWORD WINAPI file_reader_func(void* data);
