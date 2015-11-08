#pragma once

void dir(run_params* par);
void echo(run_params* par);
void scan(run_params* par);
void random(run_params* par);
void tree(run_params* par);
void tree_list(node* n, int* spaces, run_params* par);
void c_pipe(run_params* par);
void type(run_params* par);
void info(run_params* par);
void mkdir(run_params* par);
void rm(run_params* par);
void freq(run_params* par);


void cd(run_params* par, run_params* parent_params);