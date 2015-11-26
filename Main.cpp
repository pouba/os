// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main()
{
	srand((int)time(NULL));

	node* root_dir = node_create("Computer", NULL, 1);
	node* root = node_create("C:", root_dir, 1);
	node* n1 = node_create("n1", root, 0);
	node* n2 = node_create("n2", root, 0);
	node* n3 = node_create("n3", root, 0);
	node* n4 = node_create("n4", root, 1);
	node* n5 = node_create("n5", root, 1);
	node* n5a = node_create("n5a", n5, 0);
	node* n5b = node_create("n5b", n5, 0);

	char* str = "n1 - aaaaaa\nbbbb\ncccc\nfskdhjs\ndsfj\niuyffj\n\nejrwehjr\n";
	n1->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n1->content, str, strlen(str)+1);
	n1->content_len = strlen(n1->content);

	str = "n2 - 4546456\n";
	n2->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n2->content, str, strlen(str)+1);
	n2->content_len = strlen(n2->content);

	str = "n3 - ,m,mkjhseursjagfjhsdhj\n";
	n3->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n3->content, str, strlen(str)+1);
	n3->content_len = strlen(n3->content);

	str = "n4 - yighjgjhgfyufyt\n";
	n4->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n4->content, str, strlen(str) + 1);
	n4->content_len = strlen(n4->content);



	str = "n5a - 45455\n";
	n5a->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n5a->content, str, strlen(str) + 1);
	n5a->content_len = strlen(n5a->content);

	str = "n5b - 1010\n";
	n5b->content = (char*)malloc(sizeof(char) * strlen(str)+1);
	memcpy(n5b->content, str, strlen(str) + 1);
	n5b->content_len = strlen(n5b->content);


	pipe_in* in_in = (pipe_in *)malloc(sizeof(pipe_in));
	pipe_out* in_out = (pipe_out *)malloc(sizeof(pipe_out));
	pipe_create(in_in, in_out, 0, 0, 1);

	pipe_in* out_in = (pipe_in *)malloc(sizeof(pipe_in));
	pipe_out* out_out = (pipe_out *)malloc(sizeof(pipe_out));
	pipe_create(out_in, out_out, 0, 0);

	pipe_in* err_in = (pipe_in *)malloc(sizeof(pipe_in));
	pipe_out* err_out = (pipe_out *)malloc(sizeof(pipe_out));
	pipe_create(err_in, err_out, 0, 0);


	HANDLE h_in = std_reader_run(in_in);
	HANDLE h_out = std_writter_run(out_out);
	HANDLE h_err = std_writter_run(err_out);

	run_params par;
	par.cmd_name = "cmd\0";
	par.in = in_out;
	par.out = out_in;
	par.err = err_in;
	par.start_node = root;
	par.root_node = root;
	par.args = (char**)malloc(sizeof(char*) * 1);
	par.args[0] = "-main";
	par.argc = 1;
	par.secret_params = 1;

	c_run( (LPTHREAD_START_ROUTINE) c_cmd_run, &par, 0);

	WaitForSingleObject(h_out, INFINITE);
	WaitForSingleObject(h_err, INFINITE);
	TerminateThread(h_in, 0); // needs to be terminated, otherwise would wait for input forever

	free(par.args);

    return 0;
}