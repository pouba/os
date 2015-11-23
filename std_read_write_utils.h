#include "stdafx.h"

HANDLE std_reader_run(pipe_in* pipe_in);
HANDLE std_writter_run(pipe_out* pipe_out);

DWORD WINAPI std_writer_func(void* data);
DWORD WINAPI std_reader_func(void* data);
