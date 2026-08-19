#include "s21_string.h"

s21_size_t s21_strcspn(const char* str1, const char* str2) {
  char* result = s21_strpbrk(str1, str2);
  return (result ? (s21_size_t)(result - str1) : s21_strlen(str1));
}
