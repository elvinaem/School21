#include "s21_decimal.h"

int s21_truncate(s21_decimal value, s21_decimal *result) {
  int ret = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value)) {
    ret = 1;
  } else {
    int scale = s21_decimal_get_scale(value);
    int sign = s21_decimal_get_sign(value);

    s21_big_decimal big_val = s21_big_decimal_from_decimal(value);

    for (int i = 0; i < scale; i++) {
      int remainder;
      s21_big_decimal_div_10(big_val, &big_val, &remainder);
    }

    *result = s21_big_decimal_to_decimal(big_val);
    s21_decimal_set_scale(result, 0);
    s21_decimal_set_sign(result, sign);
  }

  return ret;
}