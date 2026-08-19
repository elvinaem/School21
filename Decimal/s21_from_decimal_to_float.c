#include "s21_decimal.h"

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int ret = 0;

  if (s21_validate_pointer(dst) || s21_decimal_is_invalid_format(src)) {
    ret = 1;
  } else {
    double result = 0.0;

    for (int i = 0; i < 96; i++) {
      if (s21_decimal_get_bit(src, i)) {
        result += ldexp(1.0, i);
      }
    }

    int scale = s21_decimal_get_scale(src);
    result /= pow(10.0, scale);

    if (s21_decimal_get_sign(src)) {
      result = -result;
    }

    *dst = (float)result;
  }

  return ret;
}