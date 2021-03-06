#include "stdafx.h"
#include "string.h"

#define FREQ_SIZE 256
#define SORT_SIZE 512

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
		if (c == -1) {
			break;
		}
		if (c == 26) {
			break;
		}

		double random = ((double)rand() / (double)RAND_MAX);

		sprintf_s(buffer, "%f\n", random);
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
	if (dir == NULL) {
		pipe_write_s(par->err, "Error when creating new directory.\n");
		return;
	}
}

void rm(run_params* par) {
	if (par->argc < 1) {
		pipe_write_s(par->err, "Too few parameters.\n");
		return;
	}
	char* rm_name = par->args[0];

	node* rm_node = node_get(rm_name, par->root_node, par->start_node);
	if (rm_node == NULL) {
		pipe_write_s(par->err, "File does not exist.\n");
		return;
	}

	int ret = node_delete_recursive(rm_node);
	if (ret != 0) {
		pipe_write_s(par->err, "Error on recursive deleting.\n");
	}
}

void freq(run_params* par) {
	int arr[FREQ_SIZE], i;
	char buf[50];

	for (i = 0; i < FREQ_SIZE; i++) arr[i] = 0;

	while (true) {
		int c = pipe_read(par->in);
		if (c == -1) break;

		if ((c < 0) || (c >= FREQ_SIZE)) continue;

		arr[c]++;
	}

	for (i = 0; i < FREQ_SIZE; i++) {
		if (arr[i] > 0) {
			sprintf_s(buf, "0x%hhx : %d\n", i, arr[i]);
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

int cmp_func(const void* a, const void* b);

void sort(run_params* par) {
	int i,pos, c = -1, arr_pos = 0, arr_size = 200;
	char* buf = (char*)malloc(sizeof(char) * SORT_SIZE);
	char** arr = (char**)malloc(sizeof(char *) * arr_size);

	while (true) {
		pos = 0;
		while (true) {
			c = pipe_read(par->in);
			if ((c == 10) || (c == -1)) break;
			buf[pos] = c;
			if (pos == SORT_SIZE) pos = 0; // do not accept lines longer than 500
			pos++;
		}
		buf[pos] = '\0';
		arr[arr_pos++] = buf;
		buf = (char*)malloc(sizeof(char) * SORT_SIZE);
		if (buf == NULL) goto end;

		if (arr_pos == arr_size) {
			arr_size += 200;
			arr = (char**)realloc(arr, sizeof(char*) * arr_size);
		}

		if (c == -1) break;
	}

	qsort(arr, arr_pos, sizeof(char*), cmp_func);

	for (i = 0; i < arr_pos; i++) {
		printf("%s\n", arr[i]);
	}

end: 
	for (i = 0; i < arr_pos; i++) {
		free(arr[i]);
	}
	free(arr);
	return;
}

int cmp_func(const void* a, const void* b) {
	char* ch_a = *(char**)a;
	char* ch_b = *(char**)b;

	//printf("%s - %s, %d\n", ch_a, ch_b, strcmp(ch_a, ch_b));

	return strcmp(ch_a, ch_b);
}

void wc_count(run_params* par, char* str, char* filename) {
	int i, lines = 0, words = 0, bytes = strlen(str), is_char = false;
	char out[200];
	for (i = 0; i < strlen(str); i++) {
		if (str[i] == ' ' || str[i] == '\n') {
			is_char = false;
			if (str[i] == '\n') lines++;
		}
		else {
			if (!is_char) words++;
			is_char = true;
		}
	}

	sprintf_s(out, "%d %d %d %s\n", lines, words, bytes, filename);
	pipe_write_s(par->out, out);
}

void wc(run_params* par) {
	if (par->argc >= 1) {
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

		wc_count(par, n->content, n->name);
		node_unlock(n);

	}
	else {
		int pos, c = -1, buf_size = 300;
		char* buf = (char*)malloc(sizeof(char) * buf_size);

		pos = 0;
		while (true) {
			c = pipe_read(par->in);
			if ((c == -1)) break;
			
			if (pos == buf_size - 1) {
				buf_size *= 2;
				buf = (char*)realloc(buf, sizeof(char*) * buf_size);
			}
			buf[pos] = c;
			pos++;
		}
		buf[pos] = '\0';

		wc_count(par, buf, "");
		free(buf);
		buf = NULL;
	}
}
