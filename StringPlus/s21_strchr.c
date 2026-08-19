#include "s21_string.h"

char* s21_strchr(const char* str, int c) {
  const unsigned char* ptr = (const unsigned char*)str;
  unsigned char ch = (unsigned char)c;

  char* ptr_n = S21_NULL;

  while (*ptr) {
    if (*ptr == ch) {
      ptr_n = (char*)ptr;
      break;
    }
    ptr++;
  }

  if (ch == '\0') {
    ptr_n = (char*)ptr;
  }

  return ptr_n;
}