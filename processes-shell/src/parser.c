#include "parser.h"


void command_init(struct command* curr) {
    curr->args_size = 0;
    curr->args = NULL;
    curr->special_builtin_arg = NULL;
    curr->out_stream_path = NULL;
}

struct command* parse_command(char *input_command) {
    struct command* current = (struct command*) malloc(sizeof(struct command));
    if (current == NULL) {
        DIE();
    }

    command_init(current);

    char *token, *saveptr;

    //FIXME: memory leak
    current->exec_name = strdup(strtok(strdup(input_command), " "));
    current->is_builtin = is_builtin(current->exec_name);

    if (strcmp(current->exec_name, ">" ) == 0) {
        WARN_ERR();
        return NULL;
    }

    // parse redirection
    char * redirect_search;
    if ((redirect_search = strchr(input_command, '>') ) != NULL) {
        // check if the redirection syntax is correct;

        // Only a signle > exists and there is something after it
        if (strchr(redirect_search+1, '>') != NULL || is_space_str(redirect_search+1)) {
            WARN_ERR();
            return NULL;
        }

        // Only one filename after >
        strtok(strdup(redirect_search+1), " ");
        if (strtok(NULL, " ") != NULL) {
            WARN_ERR();
            return NULL;
        }

        strtok(strdup(input_command), ">");
        current->out_stream_path = strdup(strtok(NULL, ">"));
        current->out_stream_path = trimwhitespace(current->out_stream_path);

        *redirect_search = 0;
    }

    // parse arguments
    while ((token = strtok_r(input_command, " ", &saveptr))) {
        input_command = NULL;
        current->args_size += 1;
        current->args = realloc(current->args, current->args_size*sizeof(char *));
        if (current->args == NULL) {
            DIE();
        }

        current->args[current->args_size - 1] = strdup(token);
    }


    return current;
}
void command_line_init(struct command_line* curr) {
    assert(curr != NULL);
    curr->command_list = NULL;
    curr->command_list_size = 0;
    curr->parallel_start = false;
}

void command_line_append(struct command_line* curr, struct command* new_com) {
    if (new_com == NULL) return;
    curr->command_list_size += 1;
    curr->command_list = realloc(curr->command_list, (curr->command_list_size)*sizeof(struct command*));
    if (curr->command_list == NULL){
        DIE();
    }
    curr->command_list[curr->command_list_size - 1] = new_com;
}

void print_command(struct command* curr) {
    printf("command name: %s\n", curr->exec_name);
    printf("command args: \n");
    for (int i = 0; i < curr->args_size; i++){
        printf("\t-- arg%d:%s\n", i+1, curr->args[i]);
    }
    printf("\n");
}
void print_command_line(struct command_line* curr) {
    for (int i = 0; i < curr->command_list_size; i++) {
        print_command(curr->command_list[i]);
    }
}

struct command_line* parse_command_line(char *input_line) {
    struct command_line* current = (struct command_line*) malloc(sizeof(struct command_line));
    if (current == NULL) {
        DIE();
    }

    command_line_init(current);

    // remove new line
    input_line[strcspn(input_line, "\n")] = 0;

    char * semicol_location = strchr(input_line, ';');
    char * amper_location = strchr(input_line, '&');

    char cmd_line_delimiter[2] = {0};

    if (semicol_location && amper_location) {
        WARN_ERR();
        return NULL;
    }

    if (amper_location) {
        current->parallel_start = true;
        cmd_line_delimiter[0] = '&';
    } else {
        cmd_line_delimiter[0] = ';';
    }


    char *token, *save_ptr;

    while ((token = strtok_r(input_line, cmd_line_delimiter, &save_ptr))) {
        input_line = NULL;

        // maybe show syntax error here
        if (is_space_str(token)) continue;

        command_line_append(current, parse_command(token));
    }


    return current;
}
