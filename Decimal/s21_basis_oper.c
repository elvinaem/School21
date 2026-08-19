#include "s21_decimal.h"

void s21_big_decimal_mul_accum_word(s21_big_decimal value1,
                                    s21_big_decimal value2, int i,
                                    s21_big_decimal* result) {
  s21_big_decimal temp;
  s21_big_decimal_set_zero(&temp);
  uint64_t carry = 0;
  for (int j = 0; j < 8 - i; j++) {
    uint64_t product = (uint64_t)(unsigned int)value1.bits[j] *
                           (uint64_t)(unsigned int)value2.bits[i] +
                       carry;
    temp.bits[j + i] = (unsigned int)(product & 0xFFFFFFFF);
    carry = product >> 32;
  }
  s21_big_decimal_add(*result, temp, result);
}

int s21_big_decimal_cmp(s21_big_decimal value1, s21_big_decimal value2) {
  int result = 0;
  for (int i = 7; i >= 0 && result == 0; i--) {
    unsigned int v1 = (unsigned int)value1.bits[i];
    unsigned int v2 = (unsigned int)value2.bits[i];
    if (v1 > v2) {
      result = 1;
    } else if (v1 < v2) {
      result = -1;
    }
  }
  return result;
}

int s21_big_decimal_add(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result) {
  int ret = -1;
  if (result) {
    s21_big_decimal_set_zero(result);
    uint64_t carry = 0;
    for (int i = 0; i < 8; i++) {
      uint64_t sum = (uint64_t)(unsigned int)value1.bits[i] +
                     (uint64_t)(unsigned int)value2.bits[i] + carry;
      result->bits[i] = (unsigned int)(sum & 0xFFFFFFFF);
      carry = sum >> 32;
    }
    ret = (carry != 0) ? 1 : 0;
  }
  return ret;
}

int s21_big_decimal_sub(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result) {
  int code = 0;

  if (!result) {
    code = -1;
  } else {
    s21_big_decimal_set_zero(result);

    int64_t borrow = 0;
    for (int i = 0; i < 8; i++) {
      int64_t diff = (int64_t)(uint32_t)value1.bits[i] -
                     (int64_t)(uint32_t)value2.bits[i] - borrow;

      if (diff < 0) {
        diff += 4294967296LL;
        borrow = 1;
      } else {
        borrow = 0;
      }
      result->bits[i] = (uint32_t)diff;
    }
    code = (borrow != 0);
  }

  return code;
}

int s21_big_decimal_mul(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result) {
  int ret = -1;
  if (result) {
    s21_big_decimal_set_zero(result);
    for (int i = 0; i < 8; i++) {
      if (value2.bits[i] != 0) {
        s21_big_decimal_mul_accum_word(value1, value2, i, result);
      }
    }
    ret = 0;
  }
  return ret;
}

int s21_big_decimal_div_by_one_word(s21_big_decimal value1, uint32_t divisor,
                                    s21_big_decimal* result,
                                    s21_big_decimal* remainder) {
  uint64_t rem = 0;
  for (int i = 7; i >= 0; i--) {
    uint64_t temp = (rem << 32) | (uint32_t)value1.bits[i];
    result->bits[i] = (uint32_t)(temp / divisor);
    rem = temp % divisor;
  }
  remainder->bits[0] = (uint32_t)rem;
  return 0;
}

void s21_big_div_normalization_shift(uint32_t v_top, int* s_out) {
  int s = 0;
  uint32_t x = v_top;
  if (x <= 0x0000FFFF) {
    s += 16;
    x <<= 16;
  }
  if (x <= 0x00FFFFFF) {
    s += 8;
    x <<= 8;
  }
  if (x <= 0x0FFFFFFF) {
    s += 4;
    x <<= 4;
  }
  if (x <= 0x3FFFFFFF) {
    s += 2;
    x <<= 2;
  }
  if (x <= 0x7FFFFFFF) {
    s += 1;
    x <<= 1;
  }
  *s_out = s;
}

void s21_big_div_fill_normalized_v(s21_big_decimal value2, int n, int s,
                                   uint32_t v[8]) {
  uint64_t carry = 0;
  for (int i = 0; i < n; i++) {
    uint64_t cur = (uint64_t)(uint32_t)value2.bits[i] << s;
    v[i] = (uint32_t)(cur | carry);
    carry = cur >> 32;
  }
}

void s21_big_div_fill_normalized_u(s21_big_decimal value1, int s,
                                   uint32_t u[9]) {
  uint64_t carry = 0;
  for (int i = 0; i < 8; i++) {
    uint64_t cur = (uint64_t)(uint32_t)value1.bits[i] << s;
    u[i] = (uint32_t)(cur | carry);
    carry = cur >> 32;
  }
  u[8] = (uint32_t)carry;
}

void s21_big_div_knuth_core(uint32_t u[9], const uint32_t v[8], int n,
                            s21_big_decimal* result) {
  for (int j = 8 - n; j >= 0; j--) {
    uint64_t u_high = ((uint64_t)u[j + n] << 32) | u[j + n - 1];
    uint64_t q_hat = u_high / v[n - 1];
    uint64_t r_hat = u_high % v[n - 1];

    int stop_q_adjust = 0;
    while (!stop_q_adjust &&
           (q_hat >= 0x100000000 ||
            q_hat * v[n - 2] > ((r_hat << 32) | u[j + n - 2]))) {
      q_hat--;
      r_hat += v[n - 1];
      if (r_hat >= 0x100000000) {
        stop_q_adjust = 1;
      }
    }

    int64_t borrow = 0;
    for (int i = 0; i < n; i++) {
      uint64_t p = q_hat * v[i];
      int64_t diff = (int64_t)u[j + i] - (uint32_t)p - borrow;
      u[j + i] = (uint32_t)diff;
      borrow = (int64_t)(p >> 32) - (diff >> 32);
    }
    int64_t diff_last = (int64_t)u[j + n] - borrow;
    u[j + n] = (uint32_t)diff_last;

    if (diff_last < 0) {
      q_hat--;
      uint64_t add_carry = 0;
      for (int i = 0; i < n; i++) {
        uint64_t sum = (uint64_t)u[j + i] + v[i] + add_carry;
        u[j + i] = (uint32_t)sum;
        add_carry = sum >> 32;
      }
      u[j + n] += (uint32_t)add_carry;
    }
    result->bits[j] = (uint32_t)q_hat;
  }
}

void s21_big_div_remainder_from_u(const uint32_t u[9], int n, int s,
                                  s21_big_decimal* remainder) {
  s21_big_decimal_set_zero(remainder);
  for (int i = 0; i < n; i++) {
    if (s > 0) {
      uint32_t current = u[i];
      uint32_t next = (i + 1 < 9) ? u[i + 1] : 0;
      remainder->bits[i] =
          (current >> s) | (uint32_t)((uint64_t)next << (32 - s));
    } else {
      remainder->bits[i] = u[i];
    }
  }
  for (int i = n; i < 8; i++) remainder->bits[i] = 0;
}

int s21_big_decimal_div(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result, s21_big_decimal* remainder) {
  int code = -1;

  if (!result || !remainder) {
    code = -1;
  } else {
    s21_big_decimal_set_zero(result);
    s21_big_decimal_set_zero(remainder);

    int n = 8;
    while (n > 0 && value2.bits[n - 1] == 0) n--;

    if (n == 0) {
      code = 3;
    } else if (n == 1) {
      uint32_t v_0 = (uint32_t)value2.bits[0];
      code = s21_big_decimal_div_by_one_word(value1, v_0, result, remainder);
    } else {
      int s = 0;
      uint32_t v_top = (uint32_t)value2.bits[n - 1];
      s21_big_div_normalization_shift(v_top, &s);

      uint32_t v[8] = {0};
      uint32_t u[9] = {0};
      s21_big_div_fill_normalized_v(value2, n, s, v);
      s21_big_div_fill_normalized_u(value1, s, u);

      s21_big_div_knuth_core(u, v, n, result);
      s21_big_div_remainder_from_u(u, n, s, remainder);
      code = 0;
    }
  }

  return code;
}

int s21_big_decimal_mul_10(s21_big_decimal value, s21_big_decimal* result) {
  int ret = -1;
  if (result) {
    uint64_t carry = 0;
    for (int i = 0; i < 8; i++) {
      uint64_t product = (uint64_t)(unsigned int)value.bits[i] * 10ULL + carry;
      result->bits[i] = (unsigned int)(product & 0xFFFFFFFF);
      carry = product >> 32;
    }
    ret = (carry != 0) ? 1 : 0;
  }
  return ret;
}

// value по значению; результат пишем через локальную копию, чтобы result ==
// &value было безопасно.
int s21_big_decimal_div_10(s21_big_decimal value, s21_big_decimal* result,
                           int* remainder) {
  int ret = -1;
  if (result && remainder) {
    s21_big_decimal res_tmp = {{0}};
    uint64_t temp = 0;

    for (int i = 7; i >= 0; i--) {
      temp = (temp << 32) | (uint32_t)value.bits[i];
      res_tmp.bits[i] = (uint32_t)(temp / 10);
      temp %= 10;
    }

    *result = res_tmp;
    *remainder = (int)temp;
    ret = 0;
  }
  return ret;
}

void s21_big_decimal_shift_left(s21_big_decimal* value, int shift) {
  if (value && shift > 0) {
    for (int s = 0; s < shift; s++) {
      int carry = 0;
      for (int i = 0; i < 8; i++) {
        unsigned int new_carry = ((unsigned int)value->bits[i] >> 31) & 1;
        value->bits[i] = (value->bits[i] << 1) | carry;
        carry = new_carry;
      }
    }
  }
}

void s21_align_scales_big(s21_decimal value_1, s21_decimal value_2,
                          s21_big_decimal* big1, s21_big_decimal* big2,
                          int* scale_out) {
  *big1 = s21_big_decimal_from_decimal(value_1);
  *big2 = s21_big_decimal_from_decimal(value_2);
  int s1 = s21_decimal_get_scale(value_1);
  int s2 = s21_decimal_get_scale(value_2);

  while (s1 < s2) {
    s21_big_decimal_mul_10(*big1, big1);
    s1++;
  }
  while (s2 < s1) {
    s21_big_decimal_mul_10(*big2, big2);
    s2++;
  }
  *scale_out = s1;
}
