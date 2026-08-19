

#include "s21_string.h"

static int char_to_upper(int c) {
  if (c >= 'a' && c <= 'z') {
    return c - 32;
  }
  return c;
}

void* s21_toupper(const char* str) {
  if (str == NULL) return NULL;

  size_t len = s21_strlen(str);
  char* result = malloc(len + 1);
  if (result == NULL) return NULL;

  for (size_t i = 0; i < len; i++) {
    result[i] = (char)char_to_upper((unsigned char)str[i]);
  }
  result[len] = '\0';

  return result;
}