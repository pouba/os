#include "stdafx.h"

#define MAX_CMD_LEN 300
#define MAX_PARAM_LEN 300
#define MAX_LINE_LEN 1000
#define MAX_PATH_LEN 200

#define CMD_END_CHAR 59
#define CMD_END_CHAR_LINE 10
#define CMD_END_CONCURRENT_CHAR 38

#define CMD_PATH_CMD_SEPARATOR " # "

void c_run(LPTHREAD_START_ROUTINE f, run_params* params) {
	// INIT

	// THREAD
	HANDLE thread = CreateThread(NULL, 0, f, params, 0, NULL);

	//CLEANUP

	//WAIT
	WaitForSingleObject(thread, INFINITE);
}

int parse_cmd(char* cmd, run_params* par);
int parse_line(char* line, run_params* par);
void get_path(node* node, char* path, int* position);

void dir(run_params* par);
void echo(run_params* par);
void scan(run_params* par);

void c_cmd_run(run_params* params) {
	
	int c;
	int i = 0;
	char* line = (char*)malloc(sizeof(char) * MAX_LINE_LEN);
	char* path = (char*)malloc(sizeof(char) * MAX_PATH_LEN);

	pipe* in = params->in;
	pipe* out = params->out;
	pipe* err = params->err;

	node *act = params->start_node;
	/*
	int position = 0;
	get_path(act, path, &position);
	pipe_write_s(out, path);
	pipe_write_s(out, CMD_PATH_CMD_SEPARATOR);
	*/
	i = 0;
	int pos = 0;
	while (true) {
		c = pipe_read(in);

		if (pos >= MAX_CMD_LEN) {
			pipe_write_s(err, "Maximal length of a line reached.\n");
			break;
		}

		if ((c == CMD_END_CHAR_LINE) || (c == -1)) {
			line[pos] = '\0';
			int ret = parse_line(line, params);
			if (ret == 1) break;
			pos = 0;
		}
		else {
			line[pos] = c;
			pos++;
		}
	}

}

int parse_line(char* line, run_params* par) {
	char* cmd = (char*)malloc(sizeof(char) * MAX_CMD_LEN);
	int pos_line = 0, pos_cmd = 0;
	int c;
	while (true) {
		c = line[pos_line];

		if ((c == CMD_END_CHAR) || (c == '\0')) {
			cmd[pos_cmd] = '\0';
			pos_line++;
			int ret = parse_cmd(cmd, par);
			if (ret == 1) return 1;
			pos_cmd = 0;
			if (c == '\0') return 0;
		}
		else {
			cmd[pos_cmd] = line[pos_line];
			pos_cmd++;
			pos_line++;
		}

	}
}

int fill_string(char* source, int* from, char* dest) {
	int p_pos = 0, pos = *from;

	while ((source[pos] != ' ') && (source[pos] != '\0')) {
		dest[p_pos] = source[pos];
		pos++;
		p_pos++;
	}
	dest[p_pos] = '\0';
	(*from) = pos;

	return 0;
}

int parse_cmd(char* cmd, run_params* par) {
	printf("*** %s\n", cmd);
	int pos, p_pos;

	char* cmd_itself = (char*)malloc(sizeof(char) * MAX_CMD_LEN);
	char* input = (char*)malloc(sizeof(char) * MAX_PATH_LEN);
	char* output = (char*)malloc(sizeof(char) * MAX_PATH_LEN);
	char* err_output = (char*)malloc(sizeof(char) * MAX_PATH_LEN);

	char* arg = (char*)malloc(sizeof(char) * MAX_PARAM_LEN);
	int argc = 0;

	pos = 0;
	while (cmd[pos] == ' ') pos++; // ignore leading spaces

	fill_string(cmd, &pos, cmd_itself);
	printf("cmd: %s\n", cmd_itself);

	// TODO

	while (cmd[pos] != '\0') {
		// some parameters or redirects
		while (cmd[pos] == ' ') pos++; // ignore leading spaces

		if (cmd[pos] == '<') {
			pos++;
			while (cmd[pos] == ' ') pos++; // ignore leading spaces
			if (cmd[pos] == '\0') {
				printf("Error when parsing 'in'.\n");
				return 2;
			}
			fill_string(cmd, &pos, input);
			printf("input: %s\n", input);
		}
		else if (cmd[pos] == '>') {
			pos++;
			while (cmd[pos] == ' ') pos++; // ignore leading spaces
			if (cmd[pos] == '\0') {
				printf("Error when parsing 'out'.\n");
				return 2;
			}
			fill_string(cmd, &pos, output);
			printf("output: %s\n", output);
		}
		else if (cmd[pos] == '*') {
			pos++;
			while (cmd[pos] == ' ') pos++; // ignore leading spaces
			if (cmd[pos] == '\0') {
				printf("Error when parsing 'err'.\n");
				return 2;
			}
			fill_string(cmd, &pos, err_output);
			printf("err: %s\n", err_output);
		}
		else {
			while (cmd[pos] == ' ') pos++; // ignore leading spaces
			if (cmd[pos] == '\0') {
				printf("Error when parsing 'args'.\n");
				return 2;
			}
			fill_string(cmd, &pos, arg);
			argc++;
			printf("%d. arg: %s\n", argc, arg);
		}

		//pos++;
	}





	/*
	if (strcmp(cmd, "exit") == 0) {
		return 1;
	}
	else if (strcmp(cmd, "dir") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(dir), par);
	}
	else if (strcmp(cmd, "echo") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(echo), par);
	}
	else if (strcmp(cmd, "scan") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(scan), par);
	}
	*/
	return 0;
}

void dir(run_params* par) {
	node* n = par->start_node;
	node** listDir = node_get_entries(n);
	int i = 0;
	int max = node_get_entries_count(n);
	for (i = 0; i < max; i++) {
		pipe_write_s(par->out, listDir[i]->name);
		pipe_write_s(par->out, "\n");
	}
}

void echo(run_params* par) {
	pipe_write_s(par->out, "out\n");
	pipe_write_s(par->err, "err\n");
}

void scan(run_params* par) {
	while (true) {
		int c = pipe_read(par->in);
		if (c == 65) break;

		char str[10];
		int i;
		for (i = 0; i < 10; i++) {
			str[i] = '\0';
		}
		sprintf_s(str, "%d\n", c);

		pipe_write_s(par->out, str);
	}


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