// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	node* root = node_create("C:", NULL);
	node* n1 = node_create("n1", root);
	node* n2 = node_create("n2", root);
	node* n3 = node_create("n3", root);
	node* n4 = node_create("n4", root);
	node* n5 = node_create("n5", root);
	node* n5a = node_create("n5a", n5);
	node* n5b = node_create("n5b", n5);

	pipe* in = pipe_create();
	pipe* out = pipe_create();
	pipe* err = pipe_create();

	HANDLE h_in = std_reader_run(in);
	HANDLE h_out = std_writter_run(out);
	HANDLE h_err = std_writter_run(err);

	run_params par;
	par.cmd_name = "cmd\0";
	par.in = in;
	par.out = out;
	par.err = err;
	par.start_node = root;

	c_run( (LPTHREAD_START_ROUTINE) c_cmd_run, &par);

	WaitForSingleObject(h_out, INFINITE);
	WaitForSingleObject(h_err, INFINITE);
	TerminateThread(h_in, 0); // needs to be terminated, otherwise would wait for input forever
	
    return 0;
}