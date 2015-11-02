#include "stdafx.h"

#define MAX_LINE_LEN 1000

HANDLE std_reader_run(pipe* pipe) {
	HANDLE thread = CreateThread(NULL, 0, std_reader_func, pipe, 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

HANDLE std_writter_run(pipe* pipe) {
	HANDLE thread = CreateThread(NULL, 0, std_writer_func, pipe, 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

DWORD WINAPI std_reader_func(void* data) {
	pipe* p;
	p = (pipe*)(data);

	char* line = (char*)malloc(sizeof(char) * MAX_LINE_LEN);
	int i = 0, pos = 0; 
	for (i = 0; i < MAX_LINE_LEN; i++) {
		line[i] = '\0';
	}

	int c;
	while (true) {

		c = getchar();
		pipe_write(p, c);
		if (c == -1) {
			break;
		}
	}

	return 0;
}

DWORD WINAPI std_writer_func(void* data) {
	pipe* p;
	p = (pipe*)(data);

	int c;
	while (true) {
		c  = pipe_read(p);
		if (c == -1) {
			if (p->auto_close) {
				// TODO destroy pipe
				break;
			}
			else {
				//printf("no autoclose - ignoring -1\n");
			}
		}
		else {
			putchar(c);
		}
	}

	return 0;
}

