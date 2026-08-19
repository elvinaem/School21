#include "s21_string.h"

static void fill_insert_result(char* result, const char* src, const char* str,
                               s21_size_t start_index, s21_size_t src_len,
                               s21_size_t str_len) {
  for (s21_size_t i = 0; i < start_index; i++) {
    result[i] = src[i];
  }
  for (s21_size_t i = 0; i < str_len; i++) {
    result[start_index + i] = str[i];
  }
  for (s21_size_t i = start_index; i < src_len; i++) {
    result[str_len + i] = src[i];
  }
  result[src_len + str_len] = '\0';
}

void* s21_insert(const char* src, const char* str, s21_size_t start_index) {
  char* result = NULL;

  if (src != NULL && str != NULL) {
    s21_size_t src_len = s21_strlen(src);
    s21_size_t str_len = s21_strlen(str);
    if (start_index <= src_len) {
      result = (char*)malloc(sizeof(char) * (src_len + str_len + 1));
      if (result != NULL) {
        fill_insert_result(result, src, str, start_index, src_len, str_len);
      }
    }
  }

  return result;
}
