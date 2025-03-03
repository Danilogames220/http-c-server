#include "process-files.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// dont forget to free() the string after using it!!!
char * get_file_data(size_t * fsize, const char * Path, ...) {
    // handle variadic arguments
    va_list args;
    va_start(args, Path);

    // get args size
    va_list args_copy;
    va_copy(args_copy, args);
    size_t size = vsnprintf(NULL, 0, Path, args_copy) + 1;
    va_end(args_copy);

    char *path = (char *)malloc(size * sizeof(char));
    if (path == NULL) {
        va_end(args);
    }

    vsnprintf(path, size, Path, args);
    va_end(args);


    // open file
    FILE * f = NULL;
    if ((f = fopen(path, "rb")) == NULL) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t f_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    //printf("size: %lu\n", f_size);

    char * data = malloc(sizeof(char) * f_size);
    fread(data, 1, f_size, f);
    data[f_size] = '\0';

    fclose(f);
    *fsize = f_size;
    return (char *)data;
}
