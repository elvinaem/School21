#include "s21_string.h"

void* s21_trim(const char* src, const char* trim_chars) {
  char* new_string = NULL;

  if (src != NULL && trim_chars != NULL) {
    int left_border_of_string = 0;
    while (s21_strchr(trim_chars, src[left_border_of_string]) != NULL)
      left_border_of_string += 1;

    int right_border_of_string = s21_strlen(src);
    while (s21_strchr(trim_chars, src[right_border_of_string]) != NULL)
      right_border_of_string -= 1;

    if (right_border_of_string > left_border_of_string) {
      new_string = malloc(sizeof(char) *
                          (right_border_of_string - left_border_of_string + 2));

      for (int i = left_border_of_string; i <= right_border_of_string; i++) {
        new_string[i - left_border_of_string] = src[i];
      }
      new_string[right_border_of_string - left_border_of_string + 1] = '\0';
    } else {
      new_string = malloc(sizeof(char) * 1);
      new_string[0] = '\0';
    }
  }

  return new_string;
}
