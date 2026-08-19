#include "s21_decimal.h"

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int error = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value_1) ||
      s21_decimal_is_invalid_format(value_2)) {
    error = 1;
  } else {
    s21_decimal_set_zero(result);

    int sign = s21_decimal_get_sign(value_1) ^ s21_decimal_get_sign(value_2);
    int scale = s21_decimal_get_scale(value_1) + s21_decimal_get_scale(value_2);

    s21_big_decimal big_value1 = s21_big_decimal_from_decimal(value_1);
    s21_big_decimal big_value2 = s21_big_decimal_from_decimal(value_2);
    s21_big_decimal big_result;

    s21_big_decimal_mul(big_value1, big_value2, &big_result);

    s21_big_decimal max_decimal = s21_big_decimal_max_mantissa();
    int last_rem = 0;
    int has_tail = 0;

    s21_big_decimal_downscale_excess(&big_result, &scale, &max_decimal,
                                     &last_rem, &has_tail);
    s21_big_decimal_finalize_bank_round(&big_result, &scale, last_rem, has_tail,
                                        &max_decimal);

    if (s21_big_decimal_cmp(big_result, max_decimal) > 0) {
      error = sign ? 2 : 1;
    }

    if (error == 0) {
      *result = s21_big_decimal_to_decimal(big_result);
      s21_decimal_set_scale(result, scale);
      s21_decimal_set_sign(result, sign);
    }
  }

  return error;
}
