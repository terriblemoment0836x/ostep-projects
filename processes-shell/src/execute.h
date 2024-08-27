#pragma once
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

// separate parser into parser and parser_types?
#include "parser.h"
#include "utils.h"


pid_t spawn_process(struct command* cmd, bool wait_finish);

void run_builtin(struct command* builtin_cmd);

void execute_line(struct command_line* current, char ***path);

bool locate_exec(struct command* current, char **path);
void prepare_io(struct command* cmd);

// builtin commands (maybe put them in a thier own file)

void builtin_cd(struct command* builtin_cmd);
void builtin_path(struct command* builtin_cmd);
void builtin_exit(struct command* builtin_cmd);
