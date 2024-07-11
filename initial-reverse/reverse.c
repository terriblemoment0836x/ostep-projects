#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>


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

typedef struct ll_node{
    struct ll_node *next;
    void * data;
} ll_node;

void ll_node_append(void * data, ll_node **ll) {
    if (*ll == NULL) {
        *ll = (ll_node *)malloc(sizeof(ll_node));
        if (*ll == NULL) {
            perror("ll_node_append");
            exit(1);
        }
        (*ll)->data = data;
        (*ll)->next = NULL;
    } else {
        ll_node *old_root = *ll;
        *ll = (ll_node *) malloc(sizeof(ll_node));
        if (*ll == NULL) {
            perror("ll_node_append");
            exit(1);
        }
        (*ll)->data = data;
        (*ll)->next = old_root;
    }
}

void ll_node_append_num(int n, ll_node **ll) {
    int *i = (int *)malloc(sizeof(int));
    *i = n;
    ll_node_append(i, ll);
}

void* ll_node_pop(ll_node **ll) {
    assert(*ll != NULL);
    void* saved = (*ll)->data;
    *ll = (*ll)->next;
    return saved;
}

void reverse_file(FILE* input_stream, FILE* output_stream) {
    ll_node *new_line_stack = NULL;
    ll_node_append_num(-1, &new_line_stack);

    // get new line prositions
    char buffer[256];
    while (!feof(input_stream)) {
        long curr_location = ftell(input_stream);
        size_t n = fread(buffer, sizeof(char), 256, input_stream);
        for (size_t i = 0; i < n; i++) {
            if (buffer[i] == '\n') {
                ll_node_append_num(curr_location + i, &new_line_stack);
            }
        }
    }


    while (new_line_stack != NULL) {
        ll_node_pop(&new_line_stack);
        if (new_line_stack != NULL) {
            int last_line_start = *(int *)new_line_stack->data + 1;
            fseek(input_stream, last_line_start, SEEK_SET);
            char *ptr;
            prog_getline(&ptr, input_stream);
            fprintf(output_stream,"%s", ptr);
            free(ptr);
        }
    }



    fclose(input_stream);
    fclose(output_stream);
}

void reverse_stdin_to_stdout() {
    ll_node *line_stack = NULL;
    while(!feof(stdin)) {
        char *line_ptr;
        prog_getline(&line_ptr, stdin);
        ll_node_append(line_ptr, &line_stack);
    }

    while (line_stack != NULL) {
        char * line = (char *)ll_node_pop(&line_stack);
        printf("%s", line);
    }
}

int is_same_file(FILE* file1, FILE* file2) {
    struct stat stat1, stat2;
    fstat(fileno(file1), &stat1);
    fstat(fileno(file2), &stat2);

    return stat1.st_ino == stat2.st_ino;
}
int main(int argc, char **argv) {
    FILE* input_stream,* output_stream;

    switch(argc) {
        case 1:
            if (isatty(STDIN_FILENO) == 1) 
                reverse_stdin_to_stdout();
            else
                reverse_file(stdin, stdout);
            break;
        case 2:
            input_stream = prog_open(argv[1], "r");
            output_stream = stdout;
            reverse_file(input_stream, output_stream);
            break;
        case 3:
            input_stream = prog_open(argv[1], "r");
            output_stream = prog_open(argv[2], "w");
            if(is_same_file(input_stream, output_stream)) {
                fprintf(stderr, "reverse: input and output file must differ\n");
                exit(1);
            }
            reverse_file(input_stream, output_stream);
            break;
        default:
            fprintf(stderr,"usage: reverse <input> <output>\n");
            return 1;
            break;
    }

    return 0;

}
