#include "stdafx.h"

void c_run(LPTHREAD_START_ROUTINE f, run_params* params) {
	// INIT

	// THREAD
	HANDLE thread = CreateThread(NULL, 0, f, params, 0, NULL);

	//CLEANUP

	//WAIT
	WaitForSingleObject(thread, INFINITE);
}


int dir(node* n, run_params* par);
int echo(run_params* par);
void get_path(node* node, char* path, int* position);

void c_cmd_run(run_params* params) {
	
	int c;
	int i = 0;
	char* cmd = (char*)malloc(sizeof(char) * 10);
	char* path = (char*)malloc(sizeof(char) * 200);

	pipe* in = params->in;
	pipe* out = params->out;
	pipe* err = params->err;

	node *act = params->start_node;
	int position = 0;
	get_path(act, path, &position);
	pipe_write_s(out, path);
	pipe_write_s(out, " # ");

	i = 0;
	while (true) {
		c = pipe_read(in);

		if (c == 10) {
			cmd[i] = '\0';
			i = 0;

			if (strcmp(cmd, "exit") == 0) {
				break;
			}
			if (strcmp(cmd, "dir") == 0) {
				dir(act, params);
			}
			if (strcmp(cmd, "echo") == 0) {
				echo(params);
			}

			position = 0;
			get_path(act, path, &position);
			pipe_write_s(out, path);
			pipe_write_s(out, " # ");
		}
		else {
			cmd[i] = c;
			i++;
		}
	}

}

int dir(node* n, run_params* par) {
	node** listDir = node_get_entries(n);
	int i = 0;
	int max = node_get_entries_count(n);
	for (i = 0; i < max; i++) {
		pipe_write_s(par->out, listDir[i]->name);
		pipe_write_s(par->out, "\n");
	}
	return 0;
}

int echo(run_params* par) {

	pipe_write_s(par->out, "out\n");
	pipe_write_s(par->err, "err\n");

	return 0;
}

void get_path(node* node, char* path, int* position) {
	if (node->parent != NULL) {
		get_path(node->parent, path, position);
	}
	strncpy_s(path + (sizeof(char) * (*position)), 200 - (*position), node->name, node->name_len);
	path[*position + node->name_len] = '/';
	path[*position + node->name_len + 1] = '\0';
	(*position) += node->name_len + 1;
}
