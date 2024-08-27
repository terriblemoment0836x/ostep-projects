#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "utils.h"

struct command {
    char *exec_name;
    char **args;    
    int args_size;
    bool is_builtin;
    void * special_builtin_arg;
    char* out_stream_path;
};

struct command_line {
    struct command **command_list;
    unsigned int command_list_size;
    bool parallel_start;
};

struct command* parse_command(char *input_command);
void command_init(struct command* curr);
void command_line_init(struct command_line* curr);
void command_line_append(struct command_line* curr, struct command* new_com);
void print_command(struct command* curr);
void print_command_line(struct command_line* curr);
struct command_line* parse_command_line(char *input_line);
