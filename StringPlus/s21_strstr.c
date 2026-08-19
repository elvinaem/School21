#include "s21_string.h"

char* s21_strstr(const char* haystack, const char* needle) {
  int n = s21_strlen(needle);
  char* result = 0;

  for (int i = 0; !result && haystack[i] != '\0'; i++) {
    if (haystack[i] == needle[0] && s21_strncmp(&haystack[i], needle, n) == 0) {
      result = (char*)&haystack[i];
    }
  }
  return result;
}
