#include "stdafx.h"

int pipe_create(pipe_in* pipe_in, pipe_out* pipe_out, int ac_in, int ac_out, int keyboard) {
	pipe* ret = (pipe*)malloc(sizeof(pipe));

	ret->buffer = (int*)(malloc(sizeof(int) * PIPE_BUFFER_SIZE));
	for (int i = 0; i < PIPE_BUFFER_SIZE; i++) {
		ret->buffer[i] = 0;
	}
	ret->writepos = 0;
	ret->readpos = 0;
	ret->is_keyboard = keyboard;

	ret->closed_in = 0;
	ret->closed_out = 0;

	InitializeConditionVariable(&(ret->BufferNotEmpty));
	InitializeConditionVariable(&(ret->BufferNotFull));
	InitializeCriticalSection(&(ret->BufferLock));

	pipe_in->pipe = ret;
	pipe_out->pipe = ret;

	pipe_in->autoclose = ac_in;
	pipe_out->autoclose = ac_out;

	return 0;
}

int pipe_create(pipe_in* pipe_in, pipe_out* pipe_out, int ac_in, int ac_out) {
	return pipe_create(pipe_in, pipe_out, ac_in, ac_out, 0);
}

int pipe_try_read(pipe_out* pipe_out) {
	//printf("pipe %d - try read\n", pipe_out);

	pipe* pipe = pipe_out->pipe;
	int ret;

	EnterCriticalSection(&(pipe->BufferLock));
	if (pipe->readpos == pipe->writepos) {
		ret = -2;
	}
	else {
		ret = pipe->buffer[pipe->readpos];
		pipe->readpos = (pipe->readpos + 1) % PIPE_BUFFER_SIZE;
	}

	LeaveCriticalSection(&(pipe->BufferLock));
	WakeConditionVariable(&(pipe->BufferNotFull));
	return ret;
}

int pipe_read(pipe_out* pipe_out) {
//	printf("pipe %d - read\n", pipe_out);
	pipe* pipe = pipe_out->pipe;
	int ret;

	EnterCriticalSection(&(pipe->BufferLock));
	while (pipe->readpos == pipe->writepos) {
		/* buffer is empty, sleeping */
		SleepConditionVariableCS(&(pipe->BufferNotEmpty), &(pipe->BufferLock), INFINITE);
	}
	ret = pipe->buffer[pipe->readpos];
	if (ret != -1) pipe->readpos = (pipe->readpos + 1) % PIPE_BUFFER_SIZE;

	LeaveCriticalSection(&(pipe->BufferLock));
	WakeConditionVariable(&(pipe->BufferNotFull));
	return ret;
}

void pipe_write(pipe_in* pipe_in, int wr) {
//	printf("pipe %d - write\n", pipe_in);
	pipe* pipe = pipe_in->pipe;

	EnterCriticalSection(&(pipe->BufferLock));
	int newWritePos = (pipe->writepos + 1) % PIPE_BUFFER_SIZE;

	while (pipe->readpos == newWritePos) {
		/* buffer is full, sleeping */
		SleepConditionVariableCS(&(pipe->BufferNotFull), &(pipe->BufferLock), INFINITE);
	}

	pipe->buffer[pipe->writepos] = wr;
	pipe->writepos = newWritePos;

	LeaveCriticalSection(&(pipe->BufferLock));
	WakeConditionVariable(&(pipe->BufferNotEmpty));
}

void pipe_write_s(pipe_in* pipe_in, char* s) {
	int len = strlen(s);
	for (int i = 0; i < len; i++) pipe_write(pipe_in, s[i]);
}

void pipe_close_in(pipe_in* pipe_in) {
	if (pipe_in->autoclose) {
		pipe_write(pipe_in, -1);
		if (pipe_in->pipe->closed_out == 1) {
			free(pipe_in->pipe);
		}
		else {
			pipe_in->pipe->closed_in = 1;
		}
		free(pipe_in);
	}
}

void pipe_close_out(pipe_out* pipe_out) {
	if (pipe_out->autoclose) {
		while (pipe_read(pipe_out) != -1) {};
		if (pipe_out->pipe->closed_in == 1) {
			free(pipe_out->pipe);
		}
		else {
			pipe_out->pipe->closed_out = 1;
		}
		free(pipe_out);
	}
}

int pipe_out_is_keyboard(pipe_out* pipe_out) {
	return pipe_out->pipe->is_keyboard;
}