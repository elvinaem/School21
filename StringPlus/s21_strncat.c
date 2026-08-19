#include "s21_string.h"

char* s21_strncat(char* dest, const char* src, s21_size_t n) {
  char* ptr_end = dest + s21_strlen(dest);
  int cntr = 0;

  for (s21_size_t i = 0; src[i] != '\0' && i < n; ++i) {
    ptr_end[i] = src[i];
    cntr++;
  }

  ptr_end[cntr] = '\0';

  return dest;
}
