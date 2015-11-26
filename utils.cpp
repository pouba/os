#include "stdafx.h"

void cd(run_params* par, run_params* parent_params) {
	if (par->argc < 1) return;
	char* filename = par->args[0];

	if (strcmp(filename, ".") == 0) return;
	if (strcmp(filename, "..") == 0) {
		if (parent_params->start_node->parent != NULL) {
			parent_params->start_node = parent_params->start_node->parent;
		}
		return;
	}

	node* n = node_get(filename, parent_params->root_node, parent_params->start_node);

	if (!n->directory) {
		pipe_write_s(par->err, "You can not 'cd' into a file. Please specify a directory.\n");
		return;
	}

	if (n != NULL) {
		parent_params->start_node = n;
	}
	else {
		pipe_write_s(par->err, "Folder does not exist.\n");
	}

}