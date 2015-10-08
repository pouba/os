// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

node* root;
char* path;

int ls() {

	node** listDir = node_get_entries(root);
	int i = 0;
	int max = node_get_entries_count(root);
	for (i = 0; i < max; i++) {
		printf("%s\n", listDir[i]->name);
	}

	return 0;
}

int cd() {
	printf("%s\n", path);
	return 0;
}

int main()
{

	root = node_create("C:/", NULL);

	node* n1 = node_create("n1", root);
	node* n2 = node_create("n2", root);
	node* n3 = node_create("n3", root);
	node* n4 = node_create("n4", root);
	node* n5 = node_create("n5", root);

	int c;
	int i = 0;
	char* cmd = (char*)malloc(sizeof (char) * 10);
	path = (char*)malloc(sizeof(char) * 200);

	for (i = 0; i < 200; i++) {
		path[i] = '\0';
	}

	strncpy_s(path, 200, root->name, root->name_len);
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
			if (strcmp(cmd, "ls") == 0) {
				ls();
			}
			if (strcmp(cmd, "cd") == 0) {
				cd();
			}

			printf("%s# ", path);
		}
		else {
			cmd[i] = c;
			i++;
		}
	}

    return 0;
}