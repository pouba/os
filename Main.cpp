// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	srand(time(NULL));

	node* root = node_create("C:", NULL);
	node* n1 = node_create("n1", root);
	node* n2 = node_create("n2", root);
	node* n3 = node_create("n3", root);
	node* n4 = node_create("n4", root);
	node* n5 = node_create("n5", root);
	node* n5a = node_create("n5a", n5);
	node* n5b = node_create("n5b", n5);

	char* str = "n1 - aaaaaa";
	n1->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n1->content, str, strlen(str)+1);
	n1->content_len = strlen(n1->content);

	str = "n2 - 4546456";
	n2->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n2->content, str, strlen(str)+1);
	n2->content_len = strlen(n2->content);

	str = "n3 - ,m,mkjhseursjagfjhsdhj";
	n3->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n3->content, str, strlen(str)+1);
	n3->content_len = strlen(n3->content);

	str = "n4 - yighjgjhgfyufyt";
	n4->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n4->content, str, strlen(str) + 1);
	n4->content_len = strlen(n4->content);



	str = "n5a - 45455";
	n5a->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n5a->content, str, strlen(str) + 1);
	n5a->content_len = strlen(n5a->content);

	str = "n5b - 1010";
	n5b->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n5b->content, str, strlen(str) + 1);
	n5b->content_len = strlen(n5b->content);


	pipe* in = pipe_create();
	pipe* out = pipe_create();
	pipe* err = pipe_create();
	pipe_set_auto_close(in, 0);
	pipe_set_auto_close(out, 0);
	pipe_set_auto_close(err, 0);

	HANDLE h_in = std_reader_run(in);
	HANDLE h_out = std_writter_run(out);
	HANDLE h_err = std_writter_run(err);

	run_params par;
	par.cmd_name = "cmd\0";
	par.in = in;
	par.out = out;
	par.err = err;
	par.start_node = root;
	par.root_node = root;
	par.args = (char**)malloc(sizeof(char*) * 1);
	par.args[0] = "-main";
	par.argc = 1;

	c_run( (LPTHREAD_START_ROUTINE) c_cmd_run, &par, 0);

	WaitForSingleObject(h_out, INFINITE);
	WaitForSingleObject(h_err, INFINITE);
	TerminateThread(h_in, 0); // needs to be terminated, otherwise would wait for input forever

	free(par.args);

    return 0;
}