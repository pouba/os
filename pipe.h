#pragma once

#include "stdafx.h"

#define PIPE_BUFFER_SIZE 10

typedef struct pipe_s {

	int writepos;
	int readpos;
	int* buffer;

	int is_keyboard;

	int closed_in;
	int closed_out;

	CRITICAL_SECTION BufferLock;
	CONDITION_VARIABLE BufferNotEmpty;
	CONDITION_VARIABLE BufferNotFull;

} pipe;

typedef struct pipe_in_s {
	pipe* pipe;
	int autoclose;
} pipe_in;

typedef struct pipe_out_s {
	pipe* pipe;
	int autoclose;
} pipe_out;

int pipe_create(pipe_in* in, pipe_out* out, int ac_in, int ac_out);
int pipe_create(pipe_in* pipe_in, pipe_out* pipe_out, int ac_in, int ac_out, int keyboard);

int pipe_read(pipe_out* pipe_out);
int pipe_try_read(pipe_out* pipe_out);

void pipe_write(pipe_in* pipe_in, int wr);
void pipe_write_s(pipe_in* pipe_in, char* s);

void pipe_close_in(pipe_in* pipe_in);
void pipe_close_out(pipe_out* pipe_out);

int pipe_out_is_keyboard(pipe_out* pipe_out);
