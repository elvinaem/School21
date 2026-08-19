#ifndef S21_GREP_H
#define S21_GREP_H

#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  bool e, i, v, c, l, n, h, s, f, o;
} flags_t;

typedef struct {
  flags_t flags;
  char** patterns;
  int patt_count;
  char** files;
  int count_f;
} cont;

typedef struct {
  regex_t* regexes;
  int count;
} compil_patt;

int parser(int argc, char* argv[], cont* cx);

int out_match_row(cont* cx, compil_patt* cp);
int output_match_count(cont* cx, compil_patt* cp);
int out_match_file(cont* cx, compil_patt* cp);
int count_match_rows(const char* filename, compil_patt* cp, cont* cx);
int num_out_match(cont* cx, compil_patt* cp);
int compil_patterns(cont* cx, compil_patt* cp);
void compil_patt_free(compil_patt* cp);
bool match_pattern(const char* line, compil_patt* cp, bool invert);
#endif