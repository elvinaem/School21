#include "s21_string.h"

void* s21_memchr(const void* str, int c, s21_size_t n) {
  const unsigned char* p = (const unsigned char*)str;
  void* result = S21_NULL;
  for (s21_size_t i = 0; i < n && !result; ++i) {
    if (p[i] == (unsigned char)c) {
      result = (void*)(p + i);
    }
  }
  return result;
}
