#include "s21_string.h"

char* s21_strerror(int errnum) {
  char* result = S21_NULL;

  if (errnum < 0 || errnum >= NUM_ERROR) {
    static char buffer[256] = {'\0'};
    s21_sprintf(buffer, "%s%d", UNKNOWN_ERROR, errnum);
    result = buffer;
  } else {
    ERRORS_LIST;
    result = (char*)OS_errors[errnum];
  }

  return result;
}
