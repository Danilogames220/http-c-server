#ifndef PROCESSFILES_H
#define PROCESSFILES_H
#include <stdio.h>

// dont forget to free() the string after using it!!!
extern char * get_file_data(size_t * fsize, const char * Path, ...);

#endif
