#pragma once

#include "stdafx.h"

typedef struct run_params_s {

	pipe_out* in;
	pipe_in* out;
	pipe_in* err;
	node* start_node;
	node* root_node;

	char* cmd_name;
	int argc;
	char** args;

	int secret_params;

	LPTHREAD_START_ROUTINE func;

} run_params;

void c_run(LPTHREAD_START_ROUTINE f, run_params* params, int wait);
void c_cmd_run(run_params* params);

void get_path(node* node, char* path, int* position);
void write_path(run_params* params);

int parse_cmd(char* cmd, run_params* par, run_params* parent_par, int wait);
int parse_part(char* line, run_params* par);
int parse_line(char* line, run_params* par);

int fill_string(char* source, int* from, char* dest);
int parse_redirects(char* cmd, int* pos, char* arg, char* in, char* out, char* err, int* app);
int isTextPresent(char* src, int pos, char* target);

run_params* make_params(run_params* parent_par, char* in, char* out, char* err, int app, char** args, int argc, char* cmd_name);
void* before_after(void* par);

void params_free(run_params* par);
