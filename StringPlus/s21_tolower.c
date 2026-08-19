#include "s21_string.h"

void* s21_tolower(const char* str) {
  if (str == NULL) return NULL;

  char* result = malloc(s21_strlen(str) + 1);
  if (result == NULL) return NULL;

  for (size_t i = 0; i <= s21_strlen(str); i++) {
    char c = str[i];
    if (c >= 'A' && c <= 'Z') {
      result[i] = c + 32;
    } else {
      result[i] = c;
    }
  }

  return result;
}