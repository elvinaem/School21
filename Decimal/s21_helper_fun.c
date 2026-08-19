#include "s21_decimal.h"

void s21_decimal_copy(s21_decimal *value, s21_decimal *result) {
  if (value && result) {
    memmove(result->bits, value->bits, sizeof(int) * 4);
  }
}

void s21_decimal_set_zero(s21_decimal *value) {
  if (value) {
    memset(value->bits, 0, sizeof(int) * 4);
  }
}

int s21_decimal_is_zero_int(s21_decimal value) {
  return (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0);
}

unsigned char s21_decimal_get_scale(s21_decimal value) {
  return ((unsigned int)value.bits[3] >> 16) & 0xFF;
}

int s21_decimal_set_scale(s21_decimal *value, unsigned char num) {
  value->bits[3] = (value->bits[3] & 0x8000FFFF) | (num << 16);
  return 0;
}

int s21_decimal_get_sign(s21_decimal value) {
  return (value.bits[3] >> 31) & 1;
}

int s21_decimal_set_sign(s21_decimal *value, int sign) {
  if (sign) {
    value->bits[3] |= (1 << 31);
  } else {
    value->bits[3] &= ~(1 << 31);
  }
  return 0;
}

int s21_decimal_get_bit(s21_decimal value, int num) {
  int bit = -1;
  if (num >= 0 && num <= 127) {
    bit = (value.bits[num / 32] >> (num % 32)) & 1;
  }
  return bit;
}

void s21_big_decimal_set_zero(s21_big_decimal *value) {
  if (value) {
    memset(value->bits, 0, sizeof(int) * 8);
  }
}

int s21_big_decimal_get_bit(s21_big_decimal value, int num) {
  int bit = -1;
  if (num >= 0 && num <= 255) {
    bit = (value.bits[num / 32] >> (num % 32)) & 1;
  }
  return bit;
}

void s21_big_decimal_set_bit(s21_big_decimal *value, int num, int bit) {
  if (value && num >= 0 && num <= 255) {
    int idx = num / 32;
    int pos = num % 32;
    if (bit) {
      value->bits[idx] |= (1 << pos);
    } else {
      value->bits[idx] &= ~(1 << pos);
    }
  }
}

int s21_big_decimal_is_zero(s21_big_decimal value) {
  return (value.bits[0] == 0 && value.bits[1] == 0 && value.bits[2] == 0 &&
          value.bits[3] == 0 && value.bits[4] == 0 && value.bits[5] == 0 &&
          value.bits[6] == 0 && value.bits[7] == 0);
}

s21_big_decimal s21_big_decimal_from_decimal(s21_decimal val) {
  s21_big_decimal result;
  s21_big_decimal_set_zero(&result);

  for (int i = 0; i < 3; i++) {
    result.bits[i] = val.bits[i];
  }

  return result;
}

s21_decimal s21_big_decimal_to_decimal(s21_big_decimal val) {
  s21_decimal result;
  s21_decimal_set_zero(&result);

  for (int i = 0; i < 3; i++) {
    result.bits[i] = val.bits[i];
  }

  return result;
}

s21_big_decimal s21_big_decimal_max_mantissa(void) {
  s21_big_decimal max = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};
  return max;
}

void s21_fractional_part_from_valid(s21_decimal value, s21_decimal *result) {
  int scale = s21_decimal_get_scale(value);
  s21_decimal int_part;
  s21_truncate(value, &int_part);

  s21_big_decimal big_val = s21_big_decimal_from_decimal(value);
  s21_big_decimal big_int = s21_big_decimal_from_decimal(int_part);

  for (int i = 0; i < scale; i++) {
    s21_big_decimal_mul_10(big_int, &big_int);
  }

  s21_big_decimal_sub(big_val, big_int, &big_val);
  *result = s21_big_decimal_to_decimal(big_val);
  s21_decimal_set_scale(result, scale);
  s21_decimal_set_sign(result, s21_decimal_get_sign(value));
}

void s21_get_fractional_part(s21_decimal value, s21_decimal *result) {
  if (result) {
    s21_fractional_part_from_valid(value, result);
  }
}
