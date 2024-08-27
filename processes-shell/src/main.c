#include <stdio.h>
#include <errno.h>

#include "parser.h"
#include "execute.h"
#include "utils.h"


int prog_getline_prompt(char **restrict lineptr, size_t *restrict n, FILE *restrict stream) {
    printf("wish > " );
    errno = 0;
    return getline(lineptr, n, stream);
}

int prog_getline_no_prompt(char **restrict lineptr, size_t *restrict n, FILE *restrict stream) {
    errno = 0;
    return getline(lineptr, n, stream);
}

int main(int argc, char **argv) {

    if (argc > 2) {
        DIE();
    }

    FILE* input_stream;
    if (argc == 1)  {
        input_stream = stdin;
    } else if (access(argv[1], R_OK) == 0) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            DIE();
        }
    } else {
        DIE();
    }




    int (*input_func)(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);
    if (input_stream == stdin)
        input_func = prog_getline_prompt;
    else
        input_func = prog_getline_no_prompt;

    char *input_line = NULL;
    size_t input_line_size;
    struct command_line* parsed_line;

    /* char *path[] = {strdup("/usr/local/sbin"), "/usr/local/bin", */
    /*     "/usr/sbin", "/usr/bin" */
    /*     "/sbin", "/bin", NULL}; */
    char **path = (char **) malloc(sizeof(char *) * 2);
    path[0] = strdup("/bin");
    path[1] = NULL;

    while (input_func(&input_line, &input_line_size, input_stream) != -1) {
        if (input_line[0] == '\n') continue;
        parsed_line = parse_command_line(input_line);
        if (parsed_line) execute_line(parsed_line, (char ***)&path);
    }

    if (errno != 0) {
        DIE();
    }

    fclose(input_stream);
    return 0;
}
