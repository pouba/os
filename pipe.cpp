#include "stdafx.h"

pipe* pipe_create() {
	pipe* ret = (pipe*)malloc(sizeof(pipe));

	ret->buffer = (int*)(malloc(sizeof(int) * PIPE_BUFFER_SIZE));
	for (int i = 0; i < PIPE_BUFFER_SIZE; i++) {
		ret->buffer[i] = 0;
	}
	ret->writepos = 0;
	ret->readpos = 0;
	ret->auto_close = 1;

	InitializeConditionVariable(&(ret->BufferNotEmpty));
	InitializeConditionVariable(&(ret->BufferNotFull));
	InitializeCriticalSection(&(ret->BufferLock));

	return ret;
}

void pipe_set_auto_close(pipe* pipe, int value) {
	EnterCriticalSection(&(pipe->BufferLock));
	pipe->auto_close = value;
	LeaveCriticalSection(&(pipe->BufferLock));
}

int pipe_read_non_blocking(pipe* pipe) {
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

int pipe_read(pipe* pipe) {
	int ret;

	EnterCriticalSection(&(pipe->BufferLock));
	while (pipe->readpos == pipe->writepos) {
		/* buffer is empty, sleeping */
		SleepConditionVariableCS(&(pipe->BufferNotEmpty), &(pipe->BufferLock), INFINITE);
	}
	ret = pipe->buffer[pipe->readpos];
	pipe->readpos = (pipe->readpos + 1) % PIPE_BUFFER_SIZE;

	LeaveCriticalSection(&(pipe->BufferLock));
	WakeConditionVariable(&(pipe->BufferNotFull));
	return ret;
}

void pipe_write(pipe* pipe, int wr) {
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

void pipe_write_s(pipe* pipe, char* s) {
	int len = strlen(s);
	for (int i = 0; i < len; i++) pipe_write(pipe, s[i]);
}

void pipe_d_print(pipe* pipe) {
	EnterCriticalSection(&(pipe->BufferLock));

	for (int i = 0; i < PIPE_BUFFER_SIZE; i++) {
		printf("%d, ", pipe->buffer[i]);
	}
	printf("\n");

	LeaveCriticalSection(&(pipe->BufferLock));
}