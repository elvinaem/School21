#ifndef S21_DECIMAL
#define S21_DECIMAL

#include <fenv.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BASE (0xFFFFFFFF + 1ULL)
#define MAX_BITS 3
#define MAX_BIG_BITS 7

#define S21_MAX_DECIMAL_LDOUBLE 79228162514264337593543950335.0L

typedef struct {
  int bits[4];
} s21_decimal;

typedef struct {
  int bits[8];
} s21_big_decimal;

// Основные арифметические операции
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);

// Операторы сравнения
int s21_is_less(s21_decimal value1, s21_decimal value2);
int s21_is_less_or_equal(s21_decimal value1, s21_decimal value2);
int s21_is_greater(s21_decimal value1, s21_decimal value2);
int s21_is_greater_or_equal(s21_decimal value1, s21_decimal value2);
int s21_is_equal(s21_decimal value1, s21_decimal value2);
int s21_is_not_equal(s21_decimal value1, s21_decimal value2);
int s21_decimal_cmp(s21_decimal value1, s21_decimal value2);

// Преобразователи
int s21_from_int_to_decimal(int src, s21_decimal* dst);
int s21_from_float_to_decimal(float src, s21_decimal* dst);
unsigned long long s21_from_float_bank_round_scaled(double val);
int s21_from_float_try_small(float src, s21_decimal* dst);
int s21_from_float_sci_parse(double abs_val, s21_big_decimal* big, int* scale,
                             int* has_tail);
void s21_from_float_sci_mul_until_scale_nonneg(s21_big_decimal* big,
                                               int* scale);
void s21_from_float_sci_div_until_fit(s21_big_decimal* big, int* scale,
                                      int* has_tail, int* last_rem);
void s21_from_float_sci_bank_round_last(s21_big_decimal* big, int has_tail,
                                        int last_rem);
void s21_from_float_sci_strip_trailing_zeros(s21_big_decimal* big, int* scale);
int s21_from_float_sci_write_decimal(const s21_big_decimal* big, int scale,
                                     int sign, s21_decimal* dst);
int s21_from_float_sci_reduce_write(s21_big_decimal* big, int* scale,
                                    int has_tail, int sign, s21_decimal* dst);
int s21_from_float_sci_double(double abs_val, int sign, s21_decimal* dst);
int s21_from_float_nonzero(float src, s21_decimal* dst);
int s21_from_decimal_to_int(s21_decimal src, int* dst);
int s21_from_decimal_to_float(s21_decimal src, float* dst);

// Другие функции
int s21_floor(s21_decimal value, s21_decimal* result);
int s21_round(s21_decimal value, s21_decimal* result);
int s21_truncate(s21_decimal value, s21_decimal* result);
int s21_negate(s21_decimal value, s21_decimal* result);

// Проверки аргументов (возвращают неноль при ошибке)
int s21_validate_pointer(const void* ptr);
int s21_decimal_is_invalid_format(s21_decimal value);

// Вспомогательные функции для работы с decimal
void s21_decimal_copy(s21_decimal* value, s21_decimal* result);
void s21_decimal_set_zero(s21_decimal* value);
int s21_decimal_is_zero_int(s21_decimal value);
unsigned char s21_decimal_get_scale(s21_decimal value);
int s21_decimal_set_scale(s21_decimal* value, unsigned char num);
int s21_decimal_get_sign(s21_decimal value);
int s21_decimal_set_sign(s21_decimal* value, int sign);
int s21_decimal_get_bit(s21_decimal value, int num);

// Вспомогательные функции для работы с big_decimal
s21_big_decimal s21_big_decimal_from_decimal(s21_decimal val);
s21_decimal s21_big_decimal_to_decimal(s21_big_decimal val);
void s21_big_decimal_set_zero(s21_big_decimal* value);
int s21_big_decimal_is_zero(s21_big_decimal value);
int s21_big_decimal_get_bit(s21_big_decimal value, int num);
void s21_big_decimal_set_bit(s21_big_decimal* value, int num, int bit);
int s21_big_decimal_cmp(s21_big_decimal value1, s21_big_decimal value2);
int s21_big_decimal_add(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result);
int s21_big_decimal_sub(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result);
int s21_big_decimal_mul(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result);
int s21_big_decimal_div(s21_big_decimal value1, s21_big_decimal value2,
                        s21_big_decimal* result, s21_big_decimal* remainder);
int s21_big_decimal_div_by_one_word(s21_big_decimal value1, uint32_t divisor,
                                    s21_big_decimal* result,
                                    s21_big_decimal* remainder);
void s21_big_div_normalization_shift(uint32_t v_top, int* s_out);
void s21_big_div_fill_normalized_v(s21_big_decimal value2, int n, int s,
                                   uint32_t v[8]);
void s21_big_div_fill_normalized_u(s21_big_decimal value1, int s,
                                   uint32_t u[9]);
void s21_big_div_knuth_core(uint32_t u[9], const uint32_t v[8], int n,
                            s21_big_decimal* result);
void s21_big_div_remainder_from_u(const uint32_t u[9], int n, int s,
                                  s21_big_decimal* remainder);

void s21_div_refine_quotient_mantissa(s21_big_decimal* b1, s21_big_decimal b2,
                                      s21_big_decimal* quotient,
                                      s21_big_decimal* remainder, int* scale,
                                      int* has_tail);
void s21_div_mul10_while_negative_scale(s21_big_decimal* quotient, int* scale);
void s21_div_normalize_quotient_scale(s21_big_decimal* quotient, int* scale,
                                      int* has_tail);
int s21_div_finalize(s21_big_decimal* quotient, int scale, int sign,
                     s21_decimal* result);

int s21_big_decimal_mul_10(s21_big_decimal value, s21_big_decimal* result);
int s21_big_decimal_div_10(s21_big_decimal value, s21_big_decimal* result,
                           int* remainder);
void s21_big_decimal_shift_left(s21_big_decimal* value, int shift);
// Функции нормализации и округления
void s21_align_scales_big(s21_decimal value_1, s21_decimal value_2,
                          s21_big_decimal* big1, s21_big_decimal* big2,
                          int* scale_out);
void s21_get_fractional_part(s21_decimal value, s21_decimal* result);

// Вспомогательные функции реализации (без internal linkage)
void s21_big_decimal_mul_accum_word(s21_big_decimal value1,
                                    s21_big_decimal value2, int i,
                                    s21_big_decimal* result);
s21_big_decimal s21_big_decimal_max_mantissa(void);
void s21_big_decimal_downscale_excess(s21_big_decimal* value, int* scale,
                                      const s21_big_decimal* max_mantissa,
                                      int* last_rem, int* has_tail);
void s21_big_decimal_finalize_bank_round(s21_big_decimal* value, int* scale,
                                         int last_rem, int has_tail,
                                         const s21_big_decimal* max_mantissa);
int s21_decimal_cmp_same_sign(s21_decimal value1, s21_decimal value2,
                              int sign1);

void s21_fractional_part_from_valid(s21_decimal value, s21_decimal* result);
void s21_round_adjust_integer_part(s21_decimal value, s21_decimal* result);

int s21_sub_abs(s21_decimal value_1, s21_decimal value_2, s21_decimal* result,
                int sign);
int s21_add_same_sign(s21_decimal value_1, s21_decimal value_2,
                      s21_decimal* result, int sign);

#endif
