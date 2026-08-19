#include "s21_decimal.h"

int s21_negate(s21_decimal value, s21_decimal *result) {
  int ret = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value)) {
    ret = 1;
  } else {
    s21_decimal_copy(&value, result);

    int sign = s21_decimal_get_sign(value);
    s21_decimal_set_sign(result, !sign);
  }

  return ret;
}