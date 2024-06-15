#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define READ_BUFFER_SIZE 5
#define FORMAT_PRINT_ERRNO(error_number) printf("wzip: %s\n", strerror(error_number))


FILE* open_file(char * file_path, char * mode) {
    FILE* file_ptr = fopen(file_path, mode);
    if (file_ptr == NULL) {
        int error_number = errno;
        if(error_number == ENOENT) {
            printf("wzip: cannot open file\n");
        } else {
            FORMAT_PRINT_ERRNO(error_number);
        }
        exit(1);
    }
    return file_ptr;
}


char* read_block(FILE* file_ptr) {
    assert(file_ptr != NULL);
    char *input_buffer = (char*) malloc(READ_BUFFER_SIZE*sizeof(char));
    if (input_buffer == NULL) {
            FORMAT_PRINT_ERRNO(errno);
            exit(1);
    }
    char *status = fgets(input_buffer, READ_BUFFER_SIZE, file_ptr);
    if ( status == NULL && ferror(file_ptr) != 0) {
        fprintf(stderr, "Error reading from file\n");
        exit(1);
    } 

    return status;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    FILE* file_ptr;
    char *file_path;
    char *read_data, *p;
    int i = 1;
    char current_char = 0;
    int count;

    while ( (file_path = argv[i] ) != NULL) {
        file_ptr = open_file(file_path, "r");
        assert(file_ptr != NULL);

        while ((read_data = read_block(file_ptr)) != NULL) {
            if (read_data[0] == '\0') {
                free(read_data);
                break;
            } 

            p = read_data;
            if (current_char == 0) {
                current_char = read_data[0];
                count = 1;
                p += 1;
            }


            for (; *p != '\0'; p++) {
                if (*p == current_char) {
                    ++count;
                }
                else {
                    fwrite(&count, sizeof(int), 1, stdout);
                    putchar(current_char);
                    current_char = *p;
                    count = 1;
               }
            }

            free(read_data);
        }

        fclose(file_ptr);
        i++;
    }
    fwrite(&count, sizeof(int), 1, stdout);
    putchar(current_char);

}
