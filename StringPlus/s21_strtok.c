#include "s21_string.h"

char* s21_strtok(char* str, const char* delim) {
  static char* st = S21_NULL;
  char* res = S21_NULL;

  if (str != S21_NULL) {
    st = str;
  }

  if (st != S21_NULL && *st != '\0') {
    while (*st && s21_strchr(delim, *st)) {
      st++;
    }
    if (*st != '\0') {
      res = st;
      char* c = s21_strpbrk(st, delim);
      if (c != S21_NULL) {
        *c = '\0';
        st = c + 1;
      } else {
        st = S21_NULL;
      }
    }
  }

  return res;
}