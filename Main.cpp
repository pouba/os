// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

node* root;
char* path;
node* act;

int dir() {
	node** listDir = node_get_entries(act);
	int i = 0;
	int max = node_get_entries_count(act);
	for (i = 0; i < max; i++) {
		printf("%s\n", listDir[i]->name);
	}
	return 0;
}

int down() {
	node** listDir = node_get_entries(act);
	int i = 0;
	int max = node_get_entries_count(act);

	if (max > 0) {
		act = listDir[0];
	}

	return 0;
}

int up() {
	if (act->parent != NULL) act = act->parent;
	return 0;
}

int cd() {
	printf("%s\n", path);
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

int main()
{

	root = node_create("C:", NULL);

	node* n1 = node_create("n1", root);
	node* n2 = node_create("n2", root);
	node* n3 = node_create("n3", root);
	node* n4 = node_create("n4", root);
	node* n5 = node_create("n5", root);

	node* n5a = node_create("n5a", n5);
	node* n5b = node_create("n5b", n5);

	int c;
	int i = 0;
	char* cmd = (char*)malloc(sizeof (char) * 10);
	path = (char*)malloc(sizeof(char) * 200);


	act = root;
	int position = 0;
	get_path(act, path, &position);
	printf("%s # ", path);

	i = 0;
	while (true) {
		c = getchar();

		if (c == 10) {
			cmd[i] = '\0';
			i = 0;

			if (strcmp(cmd, "exit") == 0) {
				break;
			}
			if (strcmp(cmd, "dir") == 0) {
				dir();
			}
			if (strcmp(cmd, "cd") == 0) {
				cd();
			}
			if (strcmp(cmd, "up") == 0) {
				up();
			}
			if (strcmp(cmd, "down") == 0) {
				down();
			}

			position = 0;
			get_path(act, path, &position);
			printf("%s # ", path);
		}
		else {
			cmd[i] = c;
			i++;
		}
	}
	
    return 0;
}