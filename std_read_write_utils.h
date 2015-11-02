#include "stdafx.h"

HANDLE std_reader_run(pipe* pipe);
HANDLE std_writter_run(pipe* pipe);

DWORD WINAPI std_writer_func(void* data);
DWORD WINAPI std_reader_func(void* data);
