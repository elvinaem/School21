#include "s21_string.h"

char* s21_strrchr(const char* str, int chr) {
  const char* chr_res = S21_NULL;

  if (str != S21_NULL) {
    s21_size_t i;
    for (i = 0; str[i] != '\0'; ++i) {
      if (str[i] == (char)chr) {
        chr_res = &str[i];
      }
    }
    if ((char)chr == '\0') {
      chr_res = &str[i];
    }
  }

  return (char*)chr_res;
}
