#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define READ_BUFFER_SIZE_MAG 2
#define READ_BUFFER_SIZE (READ_BUFFER_SIZE_MAG*5)
#define FORMAT_PRINT_ERRNO(error_number) printf("wunzip: %s\n", strerror(error_number))


FILE* open_file(char * file_path, char * mode) {
    FILE* file_ptr = fopen(file_path, mode);
    if (file_ptr == NULL) {
        int error_number = errno;
        if(error_number == ENOENT) {
            printf("wunzip: cannot open file\n");
        } else {
            FORMAT_PRINT_ERRNO(error_number);
        }
        exit(1);
    }
    return file_ptr;
}


int read_item(FILE* file_ptr, char* character, int *count) {
    assert(file_ptr != NULL);
    assert(character != NULL);
    assert(count != NULL);

    size_t bytes_read_int = fread(count,  1, sizeof(int),file_ptr);
    size_t bytes_read_char = fread(character, 1, sizeof(char), file_ptr);

    if (bytes_read_int != sizeof(int) || bytes_read_char != sizeof(char)){
        if (feof(file_ptr) != 0) return 0;
        else {
            FORMAT_PRINT_ERRNO(errno);
            exit(1);
        }
    }

    return 2;
}

void print_n_char(char c, unsigned int n) {
    while ( n > 0 ) {
        putchar(c);
        n--;
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }

    FILE* file_ptr;
    char *file_path;
    int i = 1;
    char current_char;
    int count;

    while ( (file_path = argv[i] ) != NULL) {
        file_ptr = open_file(file_path, "rb");
        assert(file_ptr != NULL);

        while (read_item(file_ptr, &current_char, &count) != 0) {
            print_n_char(current_char, count);
        }

        fclose(file_ptr);
        i++;
    }



}
