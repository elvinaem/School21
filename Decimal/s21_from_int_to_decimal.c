#include "s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int ret = 0;

  if (s21_validate_pointer(dst)) {
    ret = 1;
  } else {
    s21_decimal_set_zero(dst);

    if (src < 0) {
      s21_decimal_set_sign(dst, 1);
      dst->bits[0] = -src;
    } else {
      dst->bits[0] = src;
    }
  }

  return ret;
}