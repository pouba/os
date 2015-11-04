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

void cd(run_params* par) {
	printf("%d\n", par->argc);
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
		if (c == 65) {
			pipe_write(par->out, -1);
			printf("Ending\n");
			break;
		}

		pipe_write_s(par->out, "AAA\n");
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