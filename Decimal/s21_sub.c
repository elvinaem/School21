#include "s21_decimal.h"

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int error = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value_1) ||
      s21_decimal_is_invalid_format(value_2)) {
    error = 1;
  } else {
    int sign1 = s21_decimal_get_sign(value_1);
    int sign2 = s21_decimal_get_sign(value_2);

    if (s21_decimal_is_zero_int(value_1) && s21_decimal_is_zero_int(value_2)) {
      s21_decimal_set_zero(result);
      if (sign1 && !sign2) {
        s21_decimal_set_sign(result, 1);
      }
    } else if (s21_decimal_is_zero_int(value_1)) {
      *result = value_2;
      s21_decimal_set_sign(result, !sign2);
    } else if (s21_decimal_is_zero_int(value_2)) {
      *result = value_1;
    } else if (sign1 == sign2) {
      int cmp = s21_decimal_cmp(value_1, value_2);
      if (cmp == 0) {
        s21_decimal_set_zero(result);
      } else if ((!sign1 && cmp > 0) || (sign1 && cmp < 0)) {
        error = s21_sub_abs(value_1, value_2, result, sign1);
      } else {
        error = s21_sub_abs(value_2, value_1, result, !sign1);
      }
    } else if (!sign1 && sign2) {
      s21_decimal_set_sign(&value_2, 0);
      error = s21_add(value_1, value_2, result);
    } else {
      s21_decimal_set_sign(&value_2, 1);
      error = s21_add(value_1, value_2, result);
    }
  }

  return error;
}

int s21_sub_abs(s21_decimal value_1, s21_decimal value_2, s21_decimal* result,
                int sign) {
  s21_big_decimal big1, big2, big_res;
  int scale = 0;
  s21_big_decimal max_decimal = s21_big_decimal_max_mantissa();

  s21_align_scales_big(value_1, value_2, &big1, &big2, &scale);
  s21_big_decimal_sub(big1, big2, &big_res);

  int last_rem = 0;
  int has_tail = 0;

  s21_big_decimal_downscale_excess(&big_res, &scale, &max_decimal, &last_rem,
                                   &has_tail);
  s21_big_decimal_finalize_bank_round(&big_res, &scale, last_rem, has_tail,
                                      &max_decimal);

  int err = 0;
  if (s21_big_decimal_cmp(big_res, max_decimal) > 0) {
    err = sign ? 2 : 1;
  }
  if (err == 0) {
    *result = s21_big_decimal_to_decimal(big_res);
    s21_decimal_set_scale(result, scale);
    s21_decimal_set_sign(result, sign);
  }
  return err;
}
