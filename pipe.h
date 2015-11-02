#pragma once

#include "stdafx.h"

#define PIPE_BUFFER_SIZE 10

typedef struct pipe_s {

	int writepos;
	int readpos;
	int* buffer;

	int auto_close;

	CRITICAL_SECTION BufferLock;
	CONDITION_VARIABLE BufferNotEmpty;
	CONDITION_VARIABLE BufferNotFull;

} pipe;

pipe* pipe_create();

void pipe_set_auto_close(pipe* pipe, int value);

int pipe_read(pipe* pipe);
int pipe_read_non_blocking(pipe* pipe);

void pipe_write(pipe* pipe, int wr);
void pipe_write_s(pipe* pipe, char* s);
void pipe_d_print(pipe* pipe);