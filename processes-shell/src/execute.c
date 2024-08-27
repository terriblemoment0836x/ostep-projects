#include "execute.h"

void prepare_io(struct command* cmd) {
    if(cmd->out_stream_path == NULL) return;

    close(fileno(stdout));
    close(fileno(stderr));

    if (open(cmd->out_stream_path, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU) == -1) {
        WARN_ERR();
    }

    if (open(cmd->out_stream_path, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU) == -1) {
        WARN_ERR();
    }

}

void run_builtin(struct command* builtin_cmd) {
    assert(builtin_cmd->is_builtin);

    // remove the duplicated exec_name
    builtin_cmd->args++;
    builtin_cmd->args_size--;

    if (strcmp(builtin_cmd->exec_name, "cd") == 0) {
        builtin_cd(builtin_cmd);
    } else if (strcmp(builtin_cmd->exec_name, "exit") == 0) {
        builtin_exit(builtin_cmd);
    } else if (strcmp(builtin_cmd->exec_name, "path") == 0) {
        builtin_path(builtin_cmd);
    }


}


void builtin_cd(struct command* builtin_cmd) {
    if (builtin_cmd->args_size != 1) {
        WARN_ERR();
        return;
    }
    if ( chdir(builtin_cmd->args[0]) == -1 ) WARN_ERR();

}

void builtin_path(struct command* builtin_cmd) {
    assert(builtin_cmd->special_builtin_arg != NULL);

    //TODO: use typedef to hide the ***ptr
    char ***path_list_ptr = (char ***) builtin_cmd->special_builtin_arg; 
    char **new_path_list = (char **) malloc(
            sizeof(char *)*(builtin_cmd->args_size)+1);
    int i;
    for (i = 0; i < builtin_cmd->args_size; i++) {
        new_path_list[i] = strdup(builtin_cmd->args[i]);
    }
    new_path_list[i] = NULL;

    *path_list_ptr = new_path_list;
}

void builtin_exit(struct command* builtin_cmd) {
    if (builtin_cmd->args_size != 0) {
        WARN_ERR();
        return;
    }
    exit(0);
}

bool locate_exec(struct command* current, char **path) {
    assert(path != NULL);

    for ( ; *path; path++) {
        int buff_size = strlen(*path) + strlen(current->exec_name) + 3;
        assert(buff_size != 0);
        char exec_path_guess[buff_size];
        exec_path_guess[0] = 0;

        strcat(exec_path_guess, *path);
        strcat(exec_path_guess, "/");
        strcat(exec_path_guess, current->exec_name);

        if (access(exec_path_guess, R_OK | X_OK) == 0) {
            current->exec_name = strdup(exec_path_guess);
            return true; 
        }

    }

    return false;
}

pid_t spawn_process(struct command* cmd, bool parallel_start) {

    cmd->args = realloc(cmd->args, (cmd->args_size+1)*sizeof(char *));
    if (cmd->args == NULL) {
        DIE();
    }
    cmd->args[cmd->args_size] = NULL; // for execvp

    pid_t child_pid = fork();
    switch (child_pid){
        case 0:
            prepare_io(cmd);
            execv(cmd->exec_name,  cmd->args);
        case -1:
            WARN_ERR();
            return -1;
        default:
            if (!parallel_start) wait(NULL);
            return child_pid;
    }

}

void execute_line(struct command_line* current, char ***path) {
    pid_t child_pids[current->command_list_size];

    for (int i = 0; i < current->command_list_size; i++) {
        if (current->command_list[i]->is_builtin) {
            current->command_list[i]->special_builtin_arg = (void *)path;
            run_builtin(current->command_list[i]);
        }
        else if (locate_exec(current->command_list[i], *path)) {
            child_pids[i] = spawn_process(current->command_list[i],
                    current->parallel_start);
        }
        else WARN_ERR();
    }

    if (current->parallel_start) {
        for (int i = 0; i < current->command_list_size; i++){
            waitpid(child_pids[i], NULL, 0);
        }
    }
}
