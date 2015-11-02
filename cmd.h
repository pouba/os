#pragma once

#include "stdafx.h"

typedef struct run_params_s {

	pipe *in, *out, *err;
	node* start_node;
	node* root_node;

	char* cmd_name;
	int argc;
	char** args;

} run_params;

void c_run(LPTHREAD_START_ROUTINE f, run_params* params);
void c_cmd_run(run_params* params);

void get_path(node* node, char* path, int* position);