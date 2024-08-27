#pragma once
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define DIE() {\
        fprintf(stderr, "An error has occurred\n"); \
        exit(1);}

#define WARN_ERR() \
        fprintf(stderr, "An error has occurred\n");

#define PRINT_PATH(P) \
{ \
    printf("path is:"); \
    char **ptr = P; \
    while (*ptr) { \
        printf("%s:", *ptr); \
        ptr++;\
    } \
    printf("\n"); \
}

bool is_builtin(char * prog_name);

bool is_space_str(char * str);


char *trimwhitespace(char *str);
