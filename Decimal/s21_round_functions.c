#include "s21_decimal.h"

void s21_big_decimal_downscale_excess(s21_big_decimal* value, int* scale,
                                      const s21_big_decimal* max_mantissa,
                                      int* last_rem, int* has_tail) {
  while (*scale > 0 &&
         (s21_big_decimal_cmp(*value, *max_mantissa) > 0 || *scale > 28)) {
    int cur_rem = 0;
    if (*last_rem != 0) *has_tail = 1;
    s21_big_decimal_div_10(*value, value, &cur_rem);
    *last_rem = cur_rem;
    (*scale)--;
  }
}

void s21_big_decimal_finalize_bank_round(s21_big_decimal* value, int* scale,
                                         int last_rem, int has_tail,
                                         const s21_big_decimal* max_mantissa) {
  if ((last_rem > 5) || (last_rem == 5 && has_tail) ||
      (last_rem == 5 && !has_tail && (value->bits[0] % 2 != 0))) {
    s21_big_decimal one = {{1, 0, 0, 0, 0, 0, 0, 0}};
    s21_big_decimal_add(*value, one, value);
  }
  if (s21_big_decimal_cmp(*value, *max_mantissa) > 0 && *scale > 0) {
    int r = 0;
    s21_big_decimal_div_10(*value, value, &r);
    (*scale)--;
  }
}

void s21_round_adjust_integer_part(s21_decimal value, s21_decimal* result) {
  int sign = s21_decimal_get_sign(value);
  int scale = s21_decimal_get_scale(value);

  s21_decimal fractional;
  s21_get_fractional_part(value, &fractional);

  s21_big_decimal big_fractional = s21_big_decimal_from_decimal(fractional);
  s21_big_decimal big_half = {{5, 0, 0, 0, 0, 0, 0, 0}};

  for (int i = 1; i < scale; i++) {
    s21_big_decimal_mul_10(big_half, &big_half);
  }

  int cmp = s21_big_decimal_cmp(big_fractional, big_half);
  int round_up = 0;
  if (cmp >= 0) {
    round_up = 1;
  }

  if (round_up) {
    s21_decimal one = {{1, 0, 0, 0}};
    if (sign) {
      s21_sub(*result, one, result);
    } else {
      s21_add(*result, one, result);
    }
  }
}

int s21_floor(s21_decimal value, s21_decimal* result) {
  int ret = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value)) {
    ret = 1;
  } else {
    s21_truncate(value, result);

    int sign = s21_decimal_get_sign(value);
    int scale = s21_decimal_get_scale(value);

    if (sign && scale > 0) {
      s21_decimal fractional;
      s21_get_fractional_part(value, &fractional);

      if (!s21_decimal_is_zero_int(fractional)) {
        s21_decimal one = {{1, 0, 0, 0}};
        s21_sub(*result, one, result);
      }
    }
  }

  return ret;
}

int s21_round(s21_decimal value, s21_decimal* result) {
  int ret = 0;

  if (s21_validate_pointer(result) || s21_decimal_is_invalid_format(value)) {
    ret = 1;
  } else {
    s21_truncate(value, result);

    if (s21_decimal_get_scale(value) > 0) {
      s21_round_adjust_integer_part(value, result);
    }
  }

  return ret;
}
