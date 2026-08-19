#include "s21_decimal.h"

int s21_validate_pointer(const void *ptr) { return ptr == NULL; }

int s21_decimal_is_invalid_format(s21_decimal value) {
  unsigned int b = (unsigned int)value.bits[3];
  int invalid = 0;

  if ((b & 0xFFFFU) != 0U) {
    invalid = 1;
  } else if (((b >> 16) & 0xFFU) > 28U) {
    invalid = 1;
  } else if ((b & 0x7F000000U) != 0U) {
    invalid = 1;
  }

  return invalid;
}
