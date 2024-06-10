#include <stdio.h>
#include <string.h>
#include <errno.h>

#define READ_BUFFER_SIZE 100


int print_file(char * file_path, int * error_number) {
    FILE* file_ptr = fopen(file_path, "r");
    if (file_ptr == NULL) {
        *error_number = errno;
        return 1;
    }


    char read_buffer[READ_BUFFER_SIZE];
    while ( fgets(read_buffer, READ_BUFFER_SIZE, file_ptr) != NULL )
        printf("%s", read_buffer);

    if (ferror(file_ptr) != 0) {
        *error_number = -1;
        return 1;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        return 0;
    }

    char * file_path;
    int i = 1;
    int error_number;
    while ( (file_path = argv[i] ) != NULL) {
        int status =  print_file(file_path, &error_number );
        if (status != 0 && error_number == -1) {
            printf("Error happened while reading %s\n", file_path);
            return 1;
        }
        if (status != 0 ) {
            if(error_number == ENOENT) {
                printf("wcat: cannot open file\n");
            } else {
                printf("wcat: %s\n", strerror(error_number));
            }
            return 1;
        }
        
        i++;
    }

}
