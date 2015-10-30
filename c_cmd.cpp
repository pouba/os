#include "stdafx.h"

void c_run(LPTHREAD_START_ROUTINE f, run_params* params) {
	// INIT

	// THREAD
	HANDLE thread = CreateThread(NULL, 0, f, params, 0, NULL);

	//WAIT
	WaitForSingleObject(thread, INFINITE);

	//CLEANUP
	pipe_write(params->out, -1);
	pipe_write(params->err, -1);
}

int parse_cmd(char* cmd, run_params* par);
int parse_line(char* line, run_params* par);

int parse_redirects(char* cmd, int* pos, char* arg, char* in, char* out, char* err, int* app);
int isTextPresent(char* src, int pos, char* target);

run_params* make_params(run_params* parent_par, char* in, char* out, char* err, int app, char** args, int argc, char* cmd_name);

void dir(run_params* par);
void echo(run_params* par);
void scan(run_params* par);
void random(run_params* par);
void cd(run_params* par);
void tree(run_params* par);
void tree_list(node* n, int* spaces, run_params* par);
void c_pipe(run_params* par);

void c_cmd_run(run_params* params) {
	
	int c;
	int i = 0;
	char* line = (char*)malloc(sizeof(char) * MAX_LINE_LEN);
	char* path = (char*)malloc(sizeof(char) * MAX_PATH_LEN);

	pipe* in = params->in;
	pipe* out = params->out;
	pipe* err = params->err;

	node *act = params->start_node;

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
			if (c == -1) break;
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
	int in_quotes = 0;
	int i;

	while (source[pos] == ' ') pos++; // ignore leading spaces
	if (source[pos] == '\0') return 1;

	// check if text starts with any redirects
	if (isTextPresent(source, pos, STR_REDIRECT_OUT_APP)) {
		int len = strlen(STR_REDIRECT_OUT_APP);
		for (i = 0; i < len; i++) dest[i] = source[pos + i];
		dest[len] = '\0';
		pos += len;
		(*from) = pos;
		return 0;
	}
	if (isTextPresent(source, pos, STR_REDIRECT_ERR)) {
		int len = strlen(STR_REDIRECT_ERR);
		for (i = 0; i < len; i++) dest[i] = source[pos + i];
		dest[len] = '\0';
		pos += len;
		(*from) = pos;
		return 0;
	}
	if (isTextPresent(source, pos, STR_REDIRECT_IN)) {
		int len = strlen(STR_REDIRECT_IN);
		for (i = 0; i < len; i++) dest[i] = source[pos + i];
		dest[len] = '\0';
		pos += len;
		(*from) = pos;
		return 0;
	}
	if (isTextPresent(source, pos, STR_REDIRECT_OUT)) {
		int len = strlen(STR_REDIRECT_OUT);
		for (i = 0; i < len; i++) dest[i] = source[pos + i];
		dest[len] = '\0';
		pos += len;
		(*from) = pos;
		return 0;
	}

	while (source[pos] != '\0') {

		if (!in_quotes) {
			if (isTextPresent(source, pos, STR_REDIRECT_IN)) break;
			if (isTextPresent(source, pos, STR_REDIRECT_OUT)) break;
			if (isTextPresent(source, pos, STR_REDIRECT_OUT_APP)) break;
			if (isTextPresent(source, pos, STR_REDIRECT_ERR)) break;

			if (source[pos] == ' ') break;
			if (source[pos] == '"') {
				in_quotes = 1;
				pos++;
				continue;
			}
		}
		else {
			if (source[pos] == '"') {
				in_quotes = 0;
				pos++;
				continue;
			}
			if ((source[pos] == '\\') && source[pos+1] == '"') {
				pos++;
			}
		}

		dest[p_pos] = source[pos];
		pos++;
		p_pos++;
	}

	dest[p_pos] = '\0';
	(*from) = pos;

	return 0;
}

int isTextPresent(char* src, int pos, char* target) {
	int len = strlen(target);
	int i;

	for (i = 0; i < len; i++) {
		if (src[pos + i] != target[i]) return 0;
	}

return 1;
}

int parse_cmd(char* cmd, run_params* par) {
	int pos, i;

	char* cmd_itself = (char*)malloc(sizeof(char) * MAX_CMD_LEN);
	char* input = (char*)malloc(sizeof(char) * MAX_PATH_LEN);
	char* output = (char*)malloc(sizeof(char) * MAX_PATH_LEN);
	char* err_output = (char*)malloc(sizeof(char) * MAX_PATH_LEN);
	int out_append = 0;
	for (i = 0; i < MAX_PATH_LEN; i++) {
		input[i] = '\0';
		output[i] = '\0';
		err_output[i] = '\0';
	}

	char** args = (char**)malloc(sizeof(char*) * MAX_ARGC);
	char* arg = (char*)malloc(sizeof(char) * MAX_PARAM_LEN);
	int argc = 0;

	pos = 0;
	int ret;
	ret = fill_string(cmd, &pos, cmd_itself);
	if (ret != 0) return 0; // empty command

	while (1) {
		ret = fill_string(cmd, &pos, arg);
		if (ret != 0) break;

		if (parse_redirects(cmd, &pos, arg, input, output, err_output, &out_append) != 0) {
			break;
		}
		else {
			if (argc >= MAX_ARGC) {
				return 2;
			}

			args[argc] = arg;
			argc++;
			arg = (char*)malloc(sizeof(char) * MAX_PARAM_LEN);
		}
	}

	run_params* nParams = make_params(par, input, output, err_output, out_append, args, argc, cmd_itself);
	if (nParams == NULL) return 3;

	if (strcmp(cmd_itself, "exit") == 0) {
		pipe_set_auto_close(par->out, 1);
		pipe_set_auto_close(par->err, 1);
		pipe_write(par->out, -1);
		pipe_write(par->err, -1);
		return 1;
	}
	else if (strcmp(cmd_itself, "dir") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(dir), nParams);
	}
	else if (strcmp(cmd_itself, "echo") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(echo), nParams);
	}
	else if (strcmp(cmd_itself, "scan") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(scan), nParams);
	}
	else if (strcmp(cmd_itself, "rand") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(random), nParams);
	}
	else if (strcmp(cmd_itself, "cd") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(cd), nParams);
	}
	else if (strcmp(cmd_itself, "tree") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(tree), nParams);
	}
	else if (strcmp(cmd_itself, "pipe") == 0) {
		c_run((LPTHREAD_START_ROUTINE)(c_pipe), nParams);
	}

	return 0;
}

run_params* make_params(run_params* parent_par, char* in, char* out, char* err, int app, char** args, int argc, char* cmd_name) {
	run_params* nParams = (run_params*)malloc(sizeof(run_params));

	nParams->in = parent_par->in;
	nParams->out = parent_par->out;
	nParams->err = parent_par->err;

	nParams->start_node = parent_par->start_node;
	nParams->root_node = parent_par->root_node;
	nParams->cmd_name = cmd_name;

	nParams->argc = argc;
	nParams->args = args;

	if (in[0] != '\0') {
		node* node_in = get_node_by_relative_path(nParams->start_node, in);
		if (node_in == NULL) {
			printf("File not found!\n");
			return NULL;
		}
		else {
			// open file
			if (node_try_lock(node_in)) {
				pipe* p = pipe_create();
				file_reader_run(p, node_in);
				nParams->in = p;
			}
			else {
				printf("Can not open the file.\n");
				return NULL;
			}
		}
	}
	if (out[0] != '\0') {
		node* node_out = get_node_by_relative_path(nParams->start_node, out);
		if (node_out == NULL) {
			printf("File not found!\n");
			return NULL;
		}
		else {
			// open file
			if (node_try_lock(node_out)) {
				pipe* p = pipe_create();
				file_writter_run(p, node_out);
				nParams->out = p;
			}
			else {
				printf("Can not open the file.\n");
			}
		}
	}
	if (err[0] != '\0') {
		node* node_err = get_node_by_relative_path(nParams->start_node, err);
		if (node_err == NULL) {
			printf("File not found!\n");
			return NULL;
		}
		else {
			// open file
			if (node_try_lock(node_err)) {
				pipe* p = pipe_create();
				file_writter_run(p, node_err);
				nParams->out = p;
			}
			else {
				printf("Can not open the file.\n");
			}
		}
	}


	return nParams;
}

int parse_redirects(char* cmd, int* pos, char* arg, char* in, char* out, char* err, int* app) {
	int ret;
	if (strcmp(arg, STR_REDIRECT_IN) == 0) {
		ret = fill_string(cmd, pos, arg);
		if (ret != 0) return 1;
		strcpy_s(in, MAX_PATH_LEN, arg);
		return 0;
	}
	else if (strcmp(arg, STR_REDIRECT_OUT) == 0) {
		ret = fill_string(cmd, pos, arg);
		if (ret != 0) return 1;
		(*app) = 0;
		strcpy_s(out, MAX_PATH_LEN, arg);
		return 0;
	}
	else if (strcmp(arg, STR_REDIRECT_OUT_APP) == 0) {
		ret = fill_string(cmd, pos, arg);
		if (ret != 0) return 1;
		(*app) = 1;
		strcpy_s(out, MAX_PATH_LEN, arg);
		return 0;
	}
	else if (strcmp(arg, STR_REDIRECT_ERR) == 0) {
		ret = fill_string(cmd, pos, arg);
		if (ret != 0) return 1;
		strcpy_s(err, MAX_PATH_LEN, arg);
		return 0;
	}

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

void c_pipe(run_params* par) {
	int c = 0;
	while (1) {
		c = pipe_read(par->in);
		if (c == -1) break;
		pipe_write(par->out, c);
	}
}

void tree(run_params* par) {
	node* n = par->start_node;
	int spaces = 0;
	tree_list(n, &spaces, par);
}

void tree_list(node* n, int* spaces, run_params* par) {
	int i = 0;
	for (i = 0; i < (*spaces); i++) pipe_write_s(par->out, " ");
	if ((*spaces) != 0) pipe_write_s(par->out, "\\-");

	pipe_write_s(par->out, n->name);
	pipe_write_s(par->out, "\n");
	(*spaces) += 2;

	node** listDir = node_get_entries(n);
	int max = node_get_entries_count(n);
	for (i = 0; i < max; i++) {
		tree_list(n->dirEntries[i], spaces, par);
	}

	(*spaces) -= 2;
}

void cd(run_params* par) {
	char* filename = par->args[0];
	node* n = get_node_by_name(par->start_node, filename);

	if (n != NULL) {
		pipe_write_s(par->out, "found");
	}
}

void echo(run_params* par) {
	pipe_write_s(par->out, "out\n");
	pipe_write_s(par->err, "err\n");
}

void random(run_params* par) {
	while (true) {
		int c = pipe_read_non_blocking(par->in);
		if (c == 65) break;

		pipe_write_s(par->out, "AAA\n");
	}
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