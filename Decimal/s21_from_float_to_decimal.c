#include "s21_decimal.h"

unsigned long long s21_from_float_bank_round_scaled(double val) {
  unsigned long long mantissa = (unsigned long long)val;
  double fraction = val - (double)mantissa;
  unsigned long long out = mantissa;

  if (fraction > 0.5) {
    out++;
  } else if (fraction == 0.5 && (out % 2ULL) != 0ULL) {
    out++;
  }
  return out;
}

int s21_from_float_try_small(float src, s21_decimal *dst) {
  int ret = 0;
  double abs_val = fabs((double)src);
  int scale = 0;

  while (abs_val < 999999.5 && scale < 28) {
    abs_val *= 10.0;
    scale++;
  }

  unsigned long long mantissa = s21_from_float_bank_round_scaled(abs_val);
  if (mantissa >= 10000000ULL) {
    mantissa /= 10ULL;
    scale--;
  }

  s21_decimal_set_zero(dst);
  dst->bits[0] = (int)mantissa;
  s21_decimal_set_scale(dst, (unsigned char)scale);
  if (signbit((double)src)) {
    s21_decimal_set_sign(dst, 1);
  }
  return ret;
}

int s21_from_float_sci_parse(double abs_val, s21_big_decimal *big, int *scale,
                             int *has_tail) {
  int err = 0;
  char buffer[64];
  char *e_ptr = NULL;
  int exp10 = 0;
  unsigned long long mantissa = 0ULL;
  double restored = 0.0;

  (void)sprintf(buffer, "%.6E", abs_val);
  e_ptr = strchr(buffer, 'E');
  if (e_ptr == NULL) {
    err = 1;
  } else {
    (void)sscanf(e_ptr + 1, "%d", &exp10);
    for (int i = 0; i < (int)(e_ptr - buffer); i++) {
      if (buffer[i] >= '0' && buffer[i] <= '9') {
        mantissa = mantissa * 10ULL + (unsigned long long)(buffer[i] - '0');
      }
    }
    *scale = 6 - exp10;
    restored = (double)mantissa * pow(10.0, (double)(exp10 - 6));
    *has_tail = 0;
    if (abs_val > restored + (restored * 1e-9)) {
      *has_tail = 1;
    } else if (restored > abs_val + (abs_val * 1e-9)) {
      *has_tail = -1;
    }

    s21_big_decimal_set_zero(big);
    big->bits[0] = (int)(mantissa & 0xFFFFFFFFu);
    big->bits[1] = (int)((mantissa >> 32) & 0xFFFFFFFFu);
  }
  return err;
}

void s21_from_float_sci_mul_until_scale_nonneg(s21_big_decimal *big,
                                               int *scale) {
  while (*scale < 0) {
    s21_big_decimal_mul_10(*big, big);
    (*scale)++;
  }
}

void s21_from_float_sci_div_until_fit(s21_big_decimal *big, int *scale,
                                      int *has_tail, int *last_rem) {
  *last_rem = 0;
  while (*scale > 28 || (big->bits[3] || big->bits[4] || big->bits[5])) {
    if (*last_rem > 0) {
      *has_tail = 1;
    }
    s21_big_decimal_div_10(*big, big, last_rem);
    (*scale)--;
  }
}

void s21_from_float_sci_bank_round_last(s21_big_decimal *big, int has_tail,
                                        int last_rem) {
  s21_big_decimal one = {{0}};
  int need_add = 0;

  if (has_tail != -1) {
    if (last_rem > 5 ||
        (last_rem == 5 && (has_tail == 1 || (big->bits[0] & 1)))) {
      need_add = 1;
    }
  }
  if (need_add) {
    s21_big_decimal_set_zero(&one);
    one.bits[0] = 1;
    s21_big_decimal_add(*big, one, big);
  }
}

void s21_from_float_sci_strip_trailing_zeros(s21_big_decimal *big, int *scale) {
  int rem = 0;
  s21_big_decimal tmp = {{0}};
  int more = 1;

  while (*scale > 0 && more) {
    rem = 0;
    tmp = *big;
    s21_big_decimal_div_10(tmp, &tmp, &rem);
    if (rem == 0) {
      *big = tmp;
      (*scale)--;
    } else {
      more = 0;
    }
  }
}

int s21_from_float_sci_write_decimal(const s21_big_decimal *big, int scale,
                                     int sign, s21_decimal *dst) {
  int ret = 0;

  if (big->bits[3] || big->bits[4] || big->bits[5]) {
    ret = 1;
  } else {
    dst->bits[0] = big->bits[0];
    dst->bits[1] = big->bits[1];
    dst->bits[2] = big->bits[2];
    s21_decimal_set_scale(dst, (unsigned char)scale);
    s21_decimal_set_sign(dst, sign);
  }
  return ret;
}

int s21_from_float_sci_reduce_write(s21_big_decimal *big, int *scale,
                                    int has_tail, int sign, s21_decimal *dst) {
  int ret = 0;
  int last_rem = 0;

  s21_from_float_sci_mul_until_scale_nonneg(big, scale);
  s21_from_float_sci_div_until_fit(big, scale, &has_tail, &last_rem);
  s21_from_float_sci_bank_round_last(big, has_tail, last_rem);
  s21_from_float_sci_strip_trailing_zeros(big, scale);
  ret = s21_from_float_sci_write_decimal(big, *scale, sign, dst);
  return ret;
}

int s21_from_float_sci_double(double abs_val, int sign, s21_decimal *dst) {
  int ret = 0;
  s21_big_decimal big = {{0}};
  int scale = 0;
  int has_tail = 0;

  ret = s21_from_float_sci_parse(abs_val, &big, &scale, &has_tail);
  if (ret == 0) {
    ret = s21_from_float_sci_reduce_write(&big, &scale, has_tail, sign, dst);
  }
  return ret;
}

int s21_from_float_nonzero(float src, s21_decimal *dst) {
  int ret = 0;
  int sign = signbit((double)src) ? 1 : 0;
  double abs_val = fabs((double)src);

  if (abs_val < 1e-28) {
    ret = 1;
  } else if (abs_val >= (double)S21_MAX_DECIMAL_LDOUBLE) {
    ret = 1;
  } else if (abs_val < 1e-1) {
    ret = s21_from_float_try_small(src, dst);
  } else {
    ret = s21_from_float_sci_double(abs_val, sign, dst);
  }
  return ret;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  int ret = 0;

  if (s21_validate_pointer(dst)) {
    ret = 1;
  } else {
    s21_decimal_set_zero(dst);

    if (isnan(src) || isinf(src)) {
      ret = 1;
    } else if (src == 0.0f) {
      if (signbit((double)src)) {
        s21_decimal_set_sign(dst, 1);
      }
      ret = 0;
    } else {
      ret = s21_from_float_nonzero(src, dst);
    }
  }
  return ret;
}
