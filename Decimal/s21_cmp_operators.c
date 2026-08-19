#include "s21_decimal.h"

int s21_decimal_cmp_same_sign(s21_decimal value1, s21_decimal value2,
                              int sign1) {
  s21_big_decimal b1;
  s21_big_decimal b2;
  int aligned_scale = 0;
  int result = 0;

  s21_align_scales_big(value1, value2, &b1, &b2, &aligned_scale);
  result = s21_big_decimal_cmp(b1, b2);
  if (result > 0) {
    result = 1;
  } else if (result < 0) {
    result = -1;
  }
  result = sign1 ? -result : result;
  (void)aligned_scale;
  return result;
}

int s21_decimal_cmp(s21_decimal value1, s21_decimal value2) {
  int result = 0;
  if (s21_decimal_is_zero_int(value1) && s21_decimal_is_zero_int(value2)) {
    result = 0;
  } else {
    int sign1 = s21_decimal_get_sign(value1);
    int sign2 = s21_decimal_get_sign(value2);

    if (sign1 != sign2) {
      result = sign1 ? -1 : 1;
    } else {
      result = s21_decimal_cmp_same_sign(value1, value2, sign1);
    }
  }

  return result;
}

int s21_is_less(s21_decimal value1, s21_decimal value2) {
  int out = 0;
  if (!(s21_decimal_is_invalid_format(value1) ||
        s21_decimal_is_invalid_format(value2))) {
    out = (s21_decimal_cmp(value1, value2) == -1);
  }
  return out;
}

int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2) {
  int out = 0;
  if (!(s21_decimal_is_invalid_format(value1) ||
        s21_decimal_is_invalid_format(value2))) {
    int cmp_res = s21_decimal_cmp(value1, value2);
    out = (cmp_res == -1 || cmp_res == 0);
  }
  return out;
}

int s21_is_greater(s21_decimal value1, s21_decimal value2) {
  int out = 0;
  if (!(s21_decimal_is_invalid_format(value1) ||
        s21_decimal_is_invalid_format(value2))) {
    out = (s21_decimal_cmp(value1, value2) == 1);
  }
  return out;
}

int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2) {
  int out = 0;
  if (!(s21_decimal_is_invalid_format(value1) ||
        s21_decimal_is_invalid_format(value2))) {
    int cmp_res = s21_decimal_cmp(value1, value2);
    out = (cmp_res == 1 || cmp_res == 0);
  }
  return out;
}

int s21_is_equal(s21_decimal value1, s21_decimal value2) {
  int out = 0;
  if (!(s21_decimal_is_invalid_format(value1) ||
        s21_decimal_is_invalid_format(value2))) {
    out = (s21_decimal_cmp(value1, value2) == 0);
  }
  return out;
}

int s21_is_not_equal(s21_decimal value1, s21_decimal value2) {
  int out = 1;
  if (!(s21_decimal_is_invalid_format(value1) ||
        s21_decimal_is_invalid_format(value2))) {
    out = (s21_decimal_cmp(value1, value2) != 0);
  }
  return out;
}
