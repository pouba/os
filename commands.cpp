#include "stdafx.h"

void dir(run_params* par) {
	node* n = par->start_node;
	node** listDir = node_get_entries(n);
	int i = 0;
	int max = node_get_entries_count(n);
	for (i = 0; i < max; i++) {
		pipe_write_s(par->out, listDir[i]->name);
		if (listDir[i]->directory) pipe_write_s(par->out, "\\");
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
	pipe_write(par->out, '\n');
}

void random(run_params* par) {
	char buffer[50];

	while (true) {
		int c = pipe_try_read(par->in);
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
	node* n = node_get(filename, par->root_node, par->start_node);

	if (n == NULL) {
		pipe_write_s(par->err, "File does not exist.\n");
		return;
	}
	if (n->directory) {
		pipe_write_s(par->err, "File is a directory.\n");
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

void mkdir(run_params* par) {
	if (par->argc < 1) {
		pipe_write_s(par->err, "Too few parameters.\n");
		return;
	}
	char* dir_name = par->args[0];
	node* n = par->start_node;
	node** listDir = node_get_entries(n);
	int i = 0;
	int max = node_get_entries_count(n);
	for (i = 0; i < max; i++) {
		if (strcmp(dir_name, n->dirEntries[i]->name) == 0) {
			pipe_write_s(par->err, "File alrady exists.\n");
			return;
		}
	}

	node* dir = node_create(dir_name, n, 1);
}

void rm(run_params* par) {
	if (par->argc < 1) {
		pipe_write_s(par->err, "Too few parameters.\n");
		return;
	}
	char* rm_name = par->args[0];
	node* n = par->start_node;
	node* rm_node = NULL;
	node** listDir = node_get_entries(n);
	int i = 0;
	int max = node_get_entries_count(n);
	for (i = 0; i < max; i++) {
		if (strcmp(rm_name, n->dirEntries[i]->name) == 0) {
			rm_node = n->dirEntries[i];
			break;
		}
	}

	if (rm_node == 0) {
		pipe_write_s(par->err, "File does not exist.\n");
		return;
	}
	if (rm_node->directory) {
		pipe_write_s(par->err, "File is a directory.\n");
		return;
	}

	node_remove_from_dir(n, rm_node);
}

void freq(run_params* par) {
	int arr[500], i;
	char buf[50];

	for (i = 0; i < 500; i++) arr[i] = 0;

	while (true) {
		int c = pipe_read(par->in);
		if (c == -1) break;

		if ((c < 0) || (c >= 500)) continue;

		arr[c]++;
	}

	for (i = 0; i < 500; i++) {
		if (arr[i] > 0) {
			sprintf_s(buf, "%d - %d\n", i, arr[i]);
			pipe_write_s(par->out, buf);
		}
	}
}

void c_exit(run_params* par) {
	// well, basically do nothing...
}

void c_non_existent(run_params* par) {
	pipe_write_s(par->err, "Command does not exist.\n");
}