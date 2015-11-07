#include "stdafx.h"

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

void echo(run_params* par) {
	int i;

	for (i = 0; i < par->argc; i++) {
		pipe_write_s(par->out, par->args[i]);
	}
}

void random(run_params* par) {
	char buffer[50];

	while (true) {
		int c = pipe_read_non_blocking(par->in);
		if (c == 65) {
			break;
		}

		sprintf_s(buffer, "%d.%d\n", rand(), rand());
		pipe_write_s(par->out, buffer);
	}
}

void scan(run_params* par) {
	while (true) {
		int c = pipe_read(par->in);
		if (c == -1) break;

		char str[10];
		int i;
		for (i = 0; i < 10; i++) {
			str[i] = '\0';
		}
		sprintf_s(str, "%d\n", c);

		pipe_write_s(par->out, str);
	}


}

void type(run_params* par) {
	int i;

	if (par->argc < 1) {
		pipe_write_s(par->err, "Too few arguments for 'type'.\n");
		return;
	}

	char* filename = par->args[0];
	node* n = get_node_by_name(par->start_node, filename);

	if (n == NULL) {
		pipe_write_s(par->err, "File does not exist.\n");
		return;
	}

	if (!node_try_lock(n)) {
		pipe_write_s(par->err, "Can't open the file.\n");
		return;
	}

	for (i = 0; i < n->content_len; i++) {
		pipe_write(par->out, n->content[i]);
	}

	node_unlock(n);
}

void info(run_params* par) {
	int i;
	char buffer[300];

	sprintf_s(buffer, "CMD name: %s\n", par->cmd_name);
	pipe_write_s(par->out, buffer);
	sprintf_s(buffer, "Argc: %d\n", par->argc);
	pipe_write_s(par->out, buffer);

	for (i = 0; i < par->argc; i++) {
		sprintf_s(buffer, "Arg %d.: %s\n", i, par->args[i]);
		pipe_write_s(par->out, buffer);
	}
}