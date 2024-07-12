#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#define MIN(a,b) ((a>b) ? b : a)

FILE* prog_open(const char * path, const char * mode) {
    FILE* file_ptr = fopen(path, mode);

    if (file_ptr != NULL) {
        return file_ptr;
    }

    if (errno == ENOENT) {
        fprintf(stderr, "reverse: cannot open file '%s'\n", path);
    } else 
        perror("prog_open");
    exit(1);
}

ssize_t prog_getline(char **lineptr, FILE* stream) {
    *lineptr = NULL;
    size_t buff_size;
    ssize_t n = getline(lineptr, &buff_size, stream);

    if (n != -1) return n;

    if (feof(stream)) return 0;

    perror("prog_getline");
    exit(1);
}
typedef struct node node;
struct node {
    node *left;
    node *right;
    char *lineptr;   
    int len;
};

void bt_insert(node **root, char *lineptr) {
    assert(lineptr != NULL);
    if (*root == NULL) {
        *root = (node *) malloc(sizeof(node));
        if (*root == NULL) {
            perror("bt_insert");
            exit(1);
        }
        (*root)->left = NULL;
        (*root)->right = NULL;
        (*root)->lineptr = lineptr;
        (*root)->len = strlen(lineptr);
    } else if (strncmp(lineptr, (*root)->lineptr, MIN(strlen(lineptr), (*root)->len)) <= 0) {
        bt_insert(&((*root)->left), lineptr);
    } else {
        bt_insert(&((*root)->right), lineptr);
    }
}

void write_sorted_bt(node *root, FILE* stream) {
    if (root != NULL) {
        write_sorted_bt(root->left, stream);
        fprintf(stream, "%s", root->lineptr);
        write_sorted_bt(root->right, stream);
    }
}

int main(int argc, char **argv) {
    FILE *input,*output;
    if (argc > 3) {
        printf("usage: sort <input> <output>\n");
        exit(1);
    }

    input = (argc == 1) ? stdin : prog_open(argv[1], "r");
    output = (argc <= 2) ? stdout : prog_open(argv[2], "w");

    node *lines_bt = NULL;


    while(!feof(input)) {
        char *line_ptr;
        prog_getline(&line_ptr, input);
        bt_insert(&lines_bt, line_ptr);
    }

    write_sorted_bt(lines_bt, output);

    fclose(input);
    fclose(output);
    return 0;
}
