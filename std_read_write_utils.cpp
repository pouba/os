#include "stdafx.h"

#define MAX_LINE_LEN 1000

HANDLE std_reader_run(pipe_in* pipe_in) {
	HANDLE thread = CreateThread(NULL, 0, std_reader_func, pipe_in, 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

HANDLE std_writter_run(pipe_out* pipe_out) {
	HANDLE thread = CreateThread(NULL, 0, std_writer_func, pipe_out, 0, NULL);
	if (thread == NULL) printf("Error when creating thread");
	return thread;
}

DWORD WINAPI std_reader_func(void* data) {
	pipe_in* p;
	p = (pipe_in*)(data);

	_setmode(_fileno(stdin), O_BINARY);

	char* line = (char*)malloc(sizeof(char) * MAX_LINE_LEN);
	int i = 0, pos = 0; 
	for (i = 0; i < MAX_LINE_LEN; i++) {
		line[i] = '\0';
	}

	int c;
	while (true) {
		c = getchar();
		pipe_write(p, c);
	}

	return 0;
}

DWORD WINAPI std_writer_func(void* data) {
	pipe_out* p;
	p = (pipe_out*)(data);

	int c;
	while (true) {
		c  = pipe_read(p);
		if (c == -1) {
			break;
		}
		else {
			putchar(c);
		}
	}

	return 0;
}

