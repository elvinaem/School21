#define _POSIX_C_SOURCE 200809L
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct arguments {
  int b, E, n, s, T, v;

} arguments;

arguments argument_parser(int argc, char** argv);

int is_non_printable(unsigned char c);

void outline(arguments* arg, char* line, int n);

void output(arguments* arg, char* argv[], int i, int* line_count,
            char* last_simbol_prev_str);

void output_all_files(arguments* arg, int argc, char* argv[],
                      char* last_simbol_prev_str);