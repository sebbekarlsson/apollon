#ifndef APOLLON_BIN_UTILS_H
#define APOLLON_BIN_UTILS_H
/* PROG APPL: executes binary file /usr/bin/aplay. This procedure may be
 * applied to any (binary) file whatsoever. 
 *
 *                 ANDA 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h> 
 

int execute_binary(const char* filename, const char* binary_name);
#endif
