#include "s21_decimal.h"

void s21_div_refine_quotient_mantissa(s21_big_decimal* b1, s21_big_decimal b2,
                                      s21_big_decimal* quotient,
                                      s21_big_decimal* remainder, int* scale,
                                      int* has_tail) {
  int stop = 0;
  while (!stop) {
    s21_big_decimal cur_q = {0};
    s21_big_decimal tmp_q;
    s21_big_decimal_div(*b1, b2, &cur_q, remainder);
    s21_big_decimal_add(*quotient, cur_q, quotient);

    int rem_is_zero = s21_big_decimal_is_zero(*remainder);

    if (rem_is_zero || *scale >= 30) {
      if (!rem_is_zero) *has_tail = 1;
      stop = 1;
    } else if (s21_big_decimal_mul_10(*quotient, &tmp_q) != 0) {
      if (!rem_is_zero) *has_tail = 1;
      stop = 1;
    } else {
      s21_big_decimal_mul_10(*quotient, quotient);
      s21_big_decimal_mul_10(*remainder, b1);
      (*scale)++;
    }
  }
}

void s21_div_mul10_while_negative_scale(s21_big_decimal* quotient, int* scale) {
  while (*scale < 0) {
    s21_big_decimal_mul_10(*quotient, quotient);
    (*scale)++;
  }
}

void s21_div_normalize_quotient_scale(s21_big_decimal* quotient, int* scale,
                                      int* has_tail) {
  s21_big_decimal max_decimal = s21_big_decimal_max_mantissa();
  int last_rem = 0;
  s21_big_decimal_downscale_excess(quotient, scale, &max_decimal, &last_rem,
                                   has_tail);
  s21_big_decimal_finalize_bank_round(quotient, scale, last_rem, *has_tail,
                                      &max_decimal);
}

int s21_div_finalize(s21_big_decimal* quotient, int scale, int sign,
                     s21_decimal* result) {
  s21_big_decimal max_decimal = s21_big_decimal_max_mantissa();
  int error = 0;
  if (s21_big_decimal_cmp(*quotient, max_decimal) > 0) {
    error = sign ? 2 : 1;
  }

  if (error == 0) {
    *result = s21_big_decimal_to_decimal(*quotient);
    s21_decimal_set_scale(result, scale);
    s21_decimal_set_sign(result, sign);
  }

  return error;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int ret = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value_1) ||
      s21_decimal_is_invalid_format(value_2)) {
    ret = 1;
  } else if (s21_decimal_is_zero_int(value_2)) {
    ret = 3;
  } else {
    s21_decimal_set_zero(result);

    int sign = s21_decimal_get_sign(value_1) ^ s21_decimal_get_sign(value_2);
    int scale = s21_decimal_get_scale(value_1) - s21_decimal_get_scale(value_2);

    s21_big_decimal b1 = s21_big_decimal_from_decimal(value_1);
    s21_big_decimal b2 = s21_big_decimal_from_decimal(value_2);
    s21_big_decimal quotient = {0};
    s21_big_decimal remainder = {0};
    int has_tail = 0;

    s21_div_refine_quotient_mantissa(&b1, b2, &quotient, &remainder, &scale,
                                     &has_tail);
    s21_div_mul10_while_negative_scale(&quotient, &scale);
    s21_div_normalize_quotient_scale(&quotient, &scale, &has_tail);

    ret = s21_div_finalize(&quotient, scale, sign, result);
  }

  return ret;
}
