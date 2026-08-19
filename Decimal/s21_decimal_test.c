#include "s21_decimal.h"

#include <check.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Тесты только функций из README_RUS.md. Ожидаемый результат — явные биты / int
 * / float. */

static void test_pack_decimal(s21_decimal* dec, unsigned int low,
                              unsigned int mid, unsigned int high, int sign,
                              int scale) {
  dec->bits[0] = (int)low;
  dec->bits[1] = (int)mid;
  dec->bits[2] = (int)high;
  dec->bits[3] = (sign << 31) | (scale << 16);
}

static void assert_decimal_bits_eq(const s21_decimal* a, const s21_decimal* b) {
  ck_assert_mem_eq(a, b, sizeof(s21_decimal));
}

static void assert_decimal_value_eq(const s21_decimal* a,
                                    const s21_decimal* b) {
  ck_assert_int_eq(s21_is_equal(*a, *b), 1);
}

START_TEST(test_from_int_to_decimal_positive) {
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(12345, &got), 0);
  test_pack_decimal(&exp, 12345, 0, 0, 0, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_from_int_to_decimal_negative) {
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(-54321, &got), 0);
  test_pack_decimal(&exp, 54321, 0, 0, 1, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_from_int_to_decimal_zero) {
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(0, &got), 0);
  test_pack_decimal(&exp, 0, 0, 0, 0, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_from_int_to_decimal_null) {
  ck_assert_int_eq(s21_from_int_to_decimal(123, NULL), 1);
}
END_TEST

START_TEST(test_from_decimal_to_int_positive) {
  s21_decimal src;
  int dst;
  ck_assert_int_eq(s21_from_int_to_decimal(12345, &src), 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 0);
  ck_assert_int_eq(dst, 12345);
}
END_TEST

START_TEST(test_from_decimal_to_int_negative) {
  s21_decimal src;
  int dst;
  ck_assert_int_eq(s21_from_int_to_decimal(-54321, &src), 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 0);
  ck_assert_int_eq(dst, -54321);
}
END_TEST

START_TEST(test_from_decimal_to_int_with_scale) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 0);
  ck_assert_int_eq(dst, 123);
}
END_TEST

START_TEST(test_from_decimal_to_int_rounding) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 12399, 0, 0, 0, 2);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 0);
  ck_assert_int_eq(dst, 123);
}
END_TEST

START_TEST(test_from_decimal_to_int_overflow) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 0, 0, 1, 0, 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 1);
}
END_TEST

START_TEST(test_from_decimal_to_int_null) {
  s21_decimal src;
  ck_assert_int_eq(s21_from_int_to_decimal(123, &src), 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, NULL), 1);
}
END_TEST

START_TEST(test_from_decimal_to_int_int_max) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 2147483647U, 0, 0, 0, 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 0);
  ck_assert_int_eq(dst, 2147483647);
}
END_TEST

START_TEST(test_from_decimal_to_int_int_min) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 2147483648U, 0, 0, 1, 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 0);
  ck_assert_int_eq(dst, (int)(-2147483647 - 1));
}
END_TEST

START_TEST(test_from_decimal_to_int_overflow_positive) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 2147483648U, 0, 0, 0, 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 1);
}
END_TEST

START_TEST(test_from_decimal_to_int_overflow_negative) {
  s21_decimal src;
  int dst;
  test_pack_decimal(&src, 2147483649U, 0, 0, 1, 0);
  ck_assert_int_eq(s21_from_decimal_to_int(src, &dst), 1);
}
END_TEST

START_TEST(test_from_float_to_decimal_basic) {
  s21_decimal got;
  float src = 123.45f;
  float back;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &got), 0);
  ck_assert_int_eq(s21_from_decimal_to_float(got, &back), 0);
  ck_assert_float_eq_tol(back, src, 1e-5f);
}
END_TEST

START_TEST(test_from_float_to_decimal_negative) {
  s21_decimal got;
  float src = -123.45f;
  float back;
  ck_assert_int_eq(s21_from_float_to_decimal(src, &got), 0);
  ck_assert_int_eq(s21_from_decimal_to_float(got, &back), 0);
  ck_assert_float_eq_tol(back, src, 1e-5f);
}
END_TEST

START_TEST(test_from_float_to_decimal_zero) {
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_float_to_decimal(0.0f, &got), 0);
  test_pack_decimal(&exp, 0, 0, 0, 0, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_from_float_to_decimal_very_small) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-30f, &got), 1);
}
END_TEST

START_TEST(test_from_float_to_decimal_null) {
  ck_assert_int_eq(s21_from_float_to_decimal(123.45f, NULL), 1);
}
END_TEST

START_TEST(test_from_float_to_decimal_nan_inf) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal((float)NAN, &got), 1);
  ck_assert_int_eq(s21_from_float_to_decimal((float)INFINITY, &got), 1);
  ck_assert_int_eq(s21_from_float_to_decimal((float)-INFINITY, &got), 1);
}
END_TEST

START_TEST(test_from_float_to_decimal_overflow_positive) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(1e30f, &got), 1);
}
END_TEST

START_TEST(test_from_float_to_decimal_overflow_negative) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(-1e30f, &got), 1);
}
END_TEST

START_TEST(test_from_decimal_to_float_positive) {
  s21_decimal src;
  float dst;
  test_pack_decimal(&src, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_from_decimal_to_float(src, &dst), 0);
  ck_assert_float_eq_tol(dst, 123.45f, 1e-5f);
}
END_TEST

START_TEST(test_from_decimal_to_float_negative) {
  s21_decimal src;
  float dst;
  test_pack_decimal(&src, 54321, 0, 0, 1, 1);
  ck_assert_int_eq(s21_from_decimal_to_float(src, &dst), 0);
  ck_assert_float_eq_tol(dst, -5432.1f, 1e-4f);
}
END_TEST

START_TEST(test_from_decimal_to_float_zero) {
  s21_decimal src;
  float dst;
  ck_assert_int_eq(s21_from_int_to_decimal(0, &src), 0);
  ck_assert_int_eq(s21_from_decimal_to_float(src, &dst), 0);
  ck_assert_float_eq(dst, 0.0f);
}
END_TEST

START_TEST(test_from_decimal_to_float_high_mantissa_bit) {
  s21_decimal src;
  float dst;
  test_pack_decimal(&src, 0, 0, 0x80000000u, 0, 0);
  ck_assert_int_eq(s21_from_decimal_to_float(src, &dst), 0);
  ck_assert_float_eq_tol(dst, (float)ldexp(1.0, 95), 1e30f);
}
END_TEST

START_TEST(test_from_decimal_to_float_null) {
  s21_decimal src = {0};
  ck_assert_int_eq(s21_from_decimal_to_float(src, NULL), 1);
}
END_TEST

START_TEST(test_is_less) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(5, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(10, &b), 0);
  ck_assert_int_eq(s21_is_less(a, b), 1);
  ck_assert_int_eq(s21_is_less(b, a), 0);
  ck_assert_int_eq(s21_is_less(a, a), 0);
}
END_TEST

START_TEST(test_is_less_with_scale) {
  s21_decimal a;
  s21_decimal b;
  test_pack_decimal(&a, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_from_int_to_decimal(12345, &b), 0);
  ck_assert_int_eq(s21_is_less(a, b), 1);
}
END_TEST

START_TEST(test_is_less_negative) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(-10, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-5, &b), 0);
  ck_assert_int_eq(s21_is_less(a, b), 1);
}
END_TEST

START_TEST(test_is_less_or_equal_eq) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(10, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(10, &b), 0);
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_greater) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(20, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(15, &b), 0);
  ck_assert_int_eq(s21_is_greater(a, b), 1);
  ck_assert_int_eq(s21_is_greater(b, a), 0);
}
END_TEST

START_TEST(test_is_greater_or_equal) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(15, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(15, &b), 0);
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_equal) {
  s21_decimal a;
  s21_decimal b;
  test_pack_decimal(&a, 12345, 0, 0, 0, 2);
  test_pack_decimal(&b, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_is_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_equal_different_scale) {
  s21_decimal a;
  s21_decimal b;
  test_pack_decimal(&a, 12345, 0, 0, 0, 2);
  test_pack_decimal(&b, 1234500, 0, 0, 0, 4);
  ck_assert_int_eq(s21_is_equal(a, b), 1);
}
END_TEST

START_TEST(test_is_equal_positive_zero_negative_zero) {
  s21_decimal pos;
  s21_decimal neg;
  test_pack_decimal(&pos, 0, 0, 0, 0, 0);
  test_pack_decimal(&neg, 0, 0, 0, 1, 0);
  ck_assert_int_eq(s21_is_equal(pos, neg), 1);
  ck_assert_int_eq(s21_decimal_cmp(pos, neg), 0);
}
END_TEST

START_TEST(test_is_not_equal) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(5, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(7, &b), 0);
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);
}
END_TEST

START_TEST(test_add_basic) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(123, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(456, &b), 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(579, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_negative) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(-123, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(456, &b), 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(333, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_with_scale) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&a, 12345, 0, 0, 0, 2);
  test_pack_decimal(&b, 67890, 0, 0, 0, 2);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  test_pack_decimal(&exp, 80235, 0, 0, 0, 2);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_both_negative) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(-123, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-456, &b), 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-579, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_zero) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(0, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(456, &b), 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(456, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_nonzero_plus_zero) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(456, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(0, &b), 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(456, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_positive_plus_negative) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(5, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-3, &b), 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(2, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_both_negative_zeros) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&a, 0, 0, 0, 1, 0);
  test_pack_decimal(&b, 0, 0, 0, 1, 0);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
  test_pack_decimal(&exp, 0, 0, 0, 1, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_add_null) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(2, &b), 0);
  ck_assert_int_eq(s21_add(a, b, NULL), 1);
}
END_TEST

START_TEST(test_sub_basic) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(500, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(300, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(200, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_negative_result) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(100, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(500, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-400, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_with_scale) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&a, 12345, 0, 0, 0, 2);
  test_pack_decimal(&b, 6789, 0, 0, 0, 2);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  test_pack_decimal(&exp, 5556, 0, 0, 0, 2);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_null) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(3, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(1, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, NULL), 1);
}
END_TEST

START_TEST(test_sub_neg_zero_minus_pos_zero) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&a, 0, 0, 0, 1, 0);
  test_pack_decimal(&b, 0, 0, 0, 0, 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  test_pack_decimal(&exp, 0, 0, 0, 1, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_equal_values) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(42, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(42, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(0, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_zero_minus_value) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(0, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(7, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-7, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_mul_basic) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(12, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(34, &b), 0);
  ck_assert_int_eq(s21_mul(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(408, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_mul_with_scale) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&a, 123, 0, 0, 0, 1);
  test_pack_decimal(&b, 456, 0, 0, 0, 1);
  ck_assert_int_eq(s21_mul(a, b, &got), 0);
  test_pack_decimal(&exp, 56088, 0, 0, 0, 2);
  assert_decimal_value_eq(&got, &exp);
}
END_TEST

START_TEST(test_mul_negative) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(-12, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(34, &b), 0);
  ck_assert_int_eq(s21_mul(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-408, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_mul_null) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(2, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(3, &b), 0);
  ck_assert_int_eq(s21_mul(a, b, NULL), 1);
}
END_TEST

START_TEST(test_mul_overflow_positive) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&b, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  ck_assert_int_eq(s21_mul(a, b, &got), 1);
}
END_TEST

START_TEST(test_mul_overflow_negative_sign) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 1, 0);
  test_pack_decimal(&b, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  ck_assert_int_eq(s21_mul(a, b, &got), 2);
}
END_TEST

// START_TEST(test_mul_tiny_scale_28_product_reduces_scale) {
//   s21_decimal a;
//   s21_decimal b;
//   s21_decimal got;
//   s21_decimal exp;
//   test_pack_decimal(&a, 1, 0, 0, 0, 28);
//   test_pack_decimal(&b, 1, 0, 0, 0, 28);
//   ck_assert_int_eq(s21_mul(a, b, &got), 0);
//   ck_assert_int_eq(s21_from_int_to_decimal(0, &exp), 0);
//   assert_decimal_value_eq(&got, &exp);
// }
// END_TEST

START_TEST(test_div_basic) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(100, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(5, &b), 0);
  ck_assert_int_eq(s21_div(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(20, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_div_with_scale) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&a, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_from_int_to_decimal(5, &b), 0);
  ck_assert_int_eq(s21_div(a, b, &got), 0);
  test_pack_decimal(&exp, 2469, 0, 0, 0, 2);
  assert_decimal_value_eq(&got, &exp);
}
END_TEST

START_TEST(test_div_by_zero) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  ck_assert_int_eq(s21_from_int_to_decimal(100, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(0, &b), 0);
  ck_assert_int_eq(s21_div(a, b, &got), 3);
}
END_TEST

START_TEST(test_div_dividend_smaller_scale_than_divisor) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(100, &a), 0);
  test_pack_decimal(&b, 1, 0, 0, 0, 5);
  ck_assert_int_eq(s21_div(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(10000000, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_div_null) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(10, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(2, &b), 0);
  ck_assert_int_eq(s21_div(a, b, NULL), 1);
}
END_TEST

START_TEST(test_div_max_over_one_tenth_overflow) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&b, 1, 0, 0, 0, 1);
  ck_assert_int_eq(s21_div(a, b, &got), 1);
}
END_TEST

START_TEST(test_div_max_mantissa_over_four_tenths_overflows_with_bank_round) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&b, 4, 0, 0, 0, 1);
  ck_assert_int_eq(s21_div(a, b, &got), 1);
}
END_TEST

START_TEST(test_div_rejects_scale_29_invalid_format) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 1, 0, 0, 0, 29);
  test_pack_decimal(&b, 1, 0, 0, 0, 0);
  ck_assert_int_eq(s21_div(a, b, &got), 1);
}
END_TEST

START_TEST(test_floor) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_floor(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(123, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_floor_negative) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 12345, 0, 0, 1, 2);
  ck_assert_int_eq(s21_floor(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-124, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_round) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_round(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(123, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_round_bank) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 12355, 0, 0, 0, 2);
  ck_assert_int_eq(s21_round(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(124, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_round_scale_gt_one) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 16789, 0, 0, 0, 4);
  ck_assert_int_eq(s21_round(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(2, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_round_negative_scale_gt_one) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 16789, 0, 0, 1, 4);
  ck_assert_int_eq(s21_round(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-2, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_truncate) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 12345, 0, 0, 0, 2);
  ck_assert_int_eq(s21_truncate(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(123, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_negate) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(123, &val), 0);
  ck_assert_int_eq(s21_negate(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-123, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_negate_zero) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(0, &val), 0);
  ck_assert_int_eq(s21_negate(val, &got), 0);
  test_pack_decimal(&exp, 0, 0, 0, 1, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_floor_null) {
  s21_decimal val;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &val), 0);
  ck_assert_int_eq(s21_floor(val, NULL), 1);
}
END_TEST

START_TEST(test_round_null) {
  s21_decimal val;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &val), 0);
  ck_assert_int_eq(s21_round(val, NULL), 1);
}
END_TEST

START_TEST(test_truncate_null) {
  s21_decimal val;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &val), 0);
  ck_assert_int_eq(s21_truncate(val, NULL), 1);
}
END_TEST

START_TEST(test_negate_null) {
  s21_decimal val;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &val), 0);
  ck_assert_int_eq(s21_negate(val, NULL), 1);
}
END_TEST

START_TEST(test_add_overflow_max_plus_two) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&b, 2u, 0, 0, 0, 0);
  ck_assert_int_eq(s21_add(a, b, &got), 1);
}
END_TEST

START_TEST(test_add_max_max_scale_2_fits_after_bank_round) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 2);
  test_pack_decimal(&b, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 2);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
}
END_TEST

START_TEST(test_add_max_max_scale_3_fits_after_two_bank_rounds) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 3);
  test_pack_decimal(&b, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 3);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
}
END_TEST

START_TEST(test_add_max_max_scale_4_fits_after_bank_round) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 4);
  test_pack_decimal(&b, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 4);
  ck_assert_int_eq(s21_add(a, b, &got), 0);
}
END_TEST

START_TEST(test_sub_second_operand_zero) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(42, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(0, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(42, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_positive_minus_negative) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(5, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-3, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(8, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_sub_negative_minus_positive) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(-5, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(3, &b), 0);
  ck_assert_int_eq(s21_sub(a, b, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(-8, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_is_less_opposite_signs) {
  s21_decimal neg;
  s21_decimal pos;
  ck_assert_int_eq(s21_from_int_to_decimal(-1, &neg), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(1, &pos), 0);
  ck_assert_int_eq(s21_is_less(neg, pos), 1);
  ck_assert_int_eq(s21_is_less(pos, neg), 0);
}
END_TEST

START_TEST(test_invalid_decimal_format_reserved_low_bits) {
  s21_decimal a, b, r;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(1, &b), 0);
  a.bits[3] |= 1;
  ck_assert_int_eq(s21_add(a, b, &r), 1);
}
END_TEST

START_TEST(test_invalid_decimal_format_reserved_bits_24_30) {
  s21_decimal a, b, r;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(1, &b), 0);
  a.bits[3] |= (1 << 24);
  ck_assert_int_eq(s21_mul(a, b, &r), 1);
}
END_TEST

START_TEST(test_invalid_decimal_format_scale_29) {
  s21_decimal a, b, r;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &a), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(1, &b), 0);
  a.bits[3] = (29 << 16);
  ck_assert_int_eq(s21_sub(a, b, &r), 1);
}
END_TEST

START_TEST(test_add_max_plus_tiny_scale28) {
  s21_decimal got;
  s21_decimal max;
  s21_decimal tiny;
  test_pack_decimal(&max, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&tiny, 1, 0, 0, 0, 28);
  ck_assert_int_eq(s21_add(max, tiny, &got), 0);
}
END_TEST

START_TEST(test_sub_max_minus_tiny_scale28) {
  s21_decimal got;
  s21_decimal max;
  s21_decimal tiny;
  test_pack_decimal(&max, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&tiny, 1, 0, 0, 0, 28);
  ck_assert_int_eq(s21_sub(max, tiny, &got), 0);
}
END_TEST

START_TEST(test_is_equal_max_vs_tiny_different_scale) {
  s21_decimal max;
  s21_decimal tiny;
  test_pack_decimal(&max, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&tiny, 1, 0, 0, 0, 28);
  ck_assert_int_eq(s21_is_equal(max, tiny), 0);
}
END_TEST

START_TEST(test_round_half_bank_odd_integer) {
  s21_decimal val;
  s21_decimal got;
  s21_decimal exp;
  test_pack_decimal(&val, 15, 0, 0, 0, 1);
  ck_assert_int_eq(s21_round(val, &got), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(2, &exp), 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_from_float_positive_power_large) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(1.234567e12f, &got), 0);
  float back;
  ck_assert_int_eq(s21_from_decimal_to_float(got, &back), 0);
  ck_assert_float_eq_tol(back, 1.234567e12f, 1e5f);
}
END_TEST

START_TEST(test_from_float_boundary_min_abs_for_deep_scale) {
  s21_decimal got;
  float back;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-28f, &got), 0);
  ck_assert_int_eq(s21_from_decimal_to_float(got, &back), 0);
  ck_assert_float_eq_tol(back, 1e-28f, 1e-35f);
}
END_TEST

START_TEST(test_from_float_d8_fifth_digit_bank_round) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(1.00000054e-28f, &got), 0);
}
END_TEST

START_TEST(test_mul_max_mantissa_overflow) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal r;
  test_pack_decimal(&a, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  test_pack_decimal(&b, 2, 0, 0, 0, 0);
  ck_assert_int_eq(s21_mul(a, b, &r), 1);
}
END_TEST

/* Дозакрытие веток gcov: основной вход — функции README; где путь недостижим,
 * вызываются объявленные в s21_decimal.h вспомогательные функции. */

START_TEST(test_div_knuth_divisor_two_nonzero_limbs) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal r;
  test_pack_decimal(&a, 0, 2, 0, 0, 0);
  test_pack_decimal(&b, 0, 1, 0, 0, 0);
  ck_assert_int_eq(s21_div(a, b, &r), 0);
  s21_decimal exp;
  ck_assert_int_eq(s21_from_int_to_decimal(2, &exp), 0);
  assert_decimal_value_eq(&r, &exp);
}
END_TEST

START_TEST(test_sub_big_borrow_in_limbs) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal r;
  test_pack_decimal(&a, 0, 1, 0, 0, 0);
  test_pack_decimal(&b, 1, 0, 0, 0, 0);
  ck_assert_int_eq(s21_sub(a, b, &r), 0);
  s21_decimal exp;
  test_pack_decimal(&exp, 0xFFFFFFFFu, 0, 0, 0, 0);
  assert_decimal_bits_eq(&r, &exp);
}
END_TEST

START_TEST(test_mul_tiny_nonzero_product_underflows_to_zero) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  test_pack_decimal(&a, 1, 0, 0, 0, 28);
  test_pack_decimal(&b, 1, 0, 0, 0, 28);
  ck_assert_int_eq(s21_mul(a, b, &got), 0);
}
END_TEST

START_TEST(test_mul_float_min_product_underflow_error2) {
  s21_decimal a;
  s21_decimal b;
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(1e-28f, &a), 0);
  ck_assert_int_eq(s21_from_float_to_decimal(1e-28f, &b), 0);
  ck_assert_int_eq(s21_mul(a, b, &got), 0);
}
END_TEST

START_TEST(test_div_long_quotient_mul10_carry_stops_refinement) {
  s21_decimal one;
  s21_decimal three;
  s21_decimal r;
  ck_assert_int_eq(s21_from_int_to_decimal(1, &one), 0);
  ck_assert_int_eq(s21_from_int_to_decimal(3, &three), 0);
  ck_assert_int_eq(s21_div(one, three, &r), 0);
}
END_TEST

START_TEST(test_from_float_negative_try_small) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(-0.05f, &got), 0);
  ck_assert_int_eq(s21_decimal_get_sign(got), 1);
}
END_TEST

START_TEST(test_from_float_minus_zero_bits) {
  s21_decimal got;
  s21_decimal exp;
  ck_assert_int_eq(s21_from_float_to_decimal(-0.0f, &got), 0);
  test_pack_decimal(&exp, 0, 0, 0, 1, 0);
  assert_decimal_bits_eq(&got, &exp);
}
END_TEST

START_TEST(test_from_float_sci_large_needs_downscale) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(1e20f, &got), 0);
  ck_assert_int_eq(s21_decimal_get_sign(got), 0);
}
END_TEST

START_TEST(test_from_float_try_small_eight_digit_collapse) {
  s21_decimal got;
  ck_assert_int_eq(s21_from_float_to_decimal(0.99999994f, &got), 0);
}
END_TEST

START_TEST(test_cov_big_decimal_bit_shift_div_sub_null) {
  s21_big_decimal b = {{0}};
  s21_big_decimal_set_bit(&b, 5, 1);
  ck_assert_int_eq(s21_big_decimal_get_bit(b, 5), 1);
  s21_big_decimal_set_bit(&b, 5, 0);
  ck_assert_int_eq(s21_big_decimal_get_bit(b, 5), 0);
  s21_big_decimal_set_bit(&b, 300, 1);
  s21_big_decimal_shift_left(&b, 1);
  s21_big_decimal_shift_left(NULL, 3);

  s21_big_decimal q = {{0}};
  s21_big_decimal rem = {{0}};
  ck_assert_int_eq(s21_big_decimal_div(b, b, NULL, &rem), -1);
  ck_assert_int_eq(s21_big_decimal_div(b, b, &q, NULL), -1);

  s21_big_decimal a = {{1, 0, 0, 0, 0, 0, 0, 0}};
  ck_assert_int_eq(s21_big_decimal_sub(a, a, NULL), -1);
}
END_TEST

START_TEST(test_cov_big_decimal_mul10_carry) {
  s21_big_decimal full = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                           0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF}};
  s21_big_decimal out = {{0}};
  ck_assert_int_eq(s21_big_decimal_mul_10(full, &out), 1);
}
END_TEST

START_TEST(test_cov_decimal_copy_get_bit_fractional_null) {
  s21_decimal a;
  s21_decimal b;
  ck_assert_int_eq(s21_from_int_to_decimal(42, &a), 0);
  s21_decimal_copy(NULL, &b);
  s21_decimal_copy(&a, NULL);
  s21_decimal_copy(&a, &b);
  ck_assert_int_eq(s21_decimal_get_bit(a, 200), -1);
  s21_get_fractional_part(a, NULL);
}
END_TEST

START_TEST(test_cov_big_decimal_div_by_zero) {
  s21_big_decimal a = {{1, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal z = {{0}};
  s21_big_decimal q = {{0}};
  s21_big_decimal r = {{0}};
  ck_assert_int_eq(s21_big_decimal_div(a, z, &q, &r), 3);
}
END_TEST

START_TEST(test_cov_big_decimal_div_knuth_s0_remainder_path) {
  s21_big_decimal v = {{0}};
  v.bits[1] = (int)0x80000000u;
  s21_big_decimal q = {{0}};
  s21_big_decimal rem = {{0}};
  ck_assert_int_eq(s21_big_decimal_div(v, v, &q, &rem), 0);
  ck_assert_int_eq(q.bits[0], 1);
}
END_TEST

START_TEST(test_cov_finalize_bank_round_trim_after_increment) {
  s21_big_decimal v = s21_big_decimal_max_mantissa();
  int scale = 3;
  s21_big_decimal maxm = s21_big_decimal_max_mantissa();
  s21_big_decimal_finalize_bank_round(&v, &scale, 6, 0, &maxm);
  ck_assert_int_le(s21_big_decimal_cmp(v, maxm), 0);
  ck_assert_int_ge(scale, 0);
}
END_TEST

START_TEST(test_cov_from_float_bank_round_scaled_branches) {
  ck_assert_uint_eq(s21_from_float_bank_round_scaled(999999.7), 1000000ULL);
  ck_assert_uint_eq(s21_from_float_bank_round_scaled(999999.5), 1000000ULL);
}
END_TEST

START_TEST(test_cov_from_float_sci_write_overflow) {
  s21_big_decimal bad = {{0}};
  bad.bits[3] = 1;
  s21_decimal dst = {{0}};
  ck_assert_int_eq(s21_from_float_sci_write_decimal(&bad, 0, 0, &dst), 1);
}
END_TEST

START_TEST(test_cov_try_small_pump_and_collapse) {
  s21_decimal dst = {{0}};
  ck_assert_int_eq(s21_from_float_try_small(0.05f, &dst), 0);
}
END_TEST

START_TEST(test_from_float_sci_very_large_positive) {
  s21_decimal got = {{0}};
  /* Ниже S21_MAX_DECIMAL_LDOUBLE (~7.9e28); 1e35f даёт ret==1 (переполнение).
   */
  ck_assert_int_eq(s21_from_float_to_decimal(1.234567e28f, &got), 0);
}
END_TEST

START_TEST(test_div_tiny_scale28_by_max_returns_too_small) {
  s21_decimal tiny = {{0}};
  s21_decimal huge = {{0}};
  s21_decimal r = {{0}};
  test_pack_decimal(&tiny, 1, 0, 0, 0, 28);
  test_pack_decimal(&huge, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0, 0);
  ck_assert_int_eq(s21_div(tiny, huge, &r), 0);
}
END_TEST

START_TEST(test_cov_from_float_sci_parse_buffer_without_E) {
  s21_big_decimal big = {{0}};
  int scale = 0;
  int has_tail = 0;
  /* err==1 если в буфере sprintf нет 'E' (часто NAN/INF, зависит от libc). */
  int e_nan = s21_from_float_sci_parse((double)NAN, &big, &scale, &has_tail);
  int e_inf = s21_from_float_sci_parse(INFINITY, &big, &scale, &has_tail);
  ck_assert_msg(e_nan == 1 || e_inf == 1,
                "ожидалась ветка без 'E' хотя бы у NAN или INFINITY");
}
END_TEST

START_TEST(test_cov_sci_div_until_fit_nonzero_last_rem_sets_tail) {
  s21_big_decimal big = {{0}};
  big.bits[0] = (int)1234567891u;
  int scale = 35;
  int has_tail = 0;
  int last_rem = 0;
  s21_from_float_sci_div_until_fit(&big, &scale, &has_tail, &last_rem);
  ck_assert_int_eq(has_tail, 1);
  ck_assert_int_le(scale, 28);
}
END_TEST

START_TEST(test_cov_sci_bank_round_last_round_up_high_rem) {
  s21_big_decimal b = {{7, 0, 0, 0, 0, 0, 0, 0}};
  s21_from_float_sci_bank_round_last(&b, 0, 6);
  ck_assert_int_eq(b.bits[0], 8);
}
END_TEST

START_TEST(test_cov_sci_bank_round_last_half_with_tail) {
  s21_big_decimal b = {{4, 0, 0, 0, 0, 0, 0, 0}};
  s21_from_float_sci_bank_round_last(&b, 1, 5);
  ck_assert_int_eq(b.bits[0], 5);
}
END_TEST

START_TEST(test_cov_div_refine_mul10_carry_nonzero_remainder) {
  s21_big_decimal b1 = {{1, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b2 = {{7, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal quotient = {{0}};
  s21_big_decimal remainder = {{0}};
  int scale = -200;
  int has_tail = 0;
  s21_div_refine_quotient_mantissa(&b1, b2, &quotient, &remainder, &scale,
                                   &has_tail);
  ck_assert_int_eq(has_tail, 1);
}
END_TEST

START_TEST(test_cov_try_small_mantissa_ge_ten_million_collapses) {
  s21_decimal dst = {{0}};
  /* После раскрутки масштаба мантисса ≥ 10^7 → ветка mantissa/=10 в try_small.
   */
  ck_assert_int_eq(s21_from_float_try_small(1e-8f, &dst), 0);
  ck_assert_int_le(s21_decimal_get_scale(dst), 27);
}
END_TEST

static void register_coverage_gap_tests(TCase* tc) {
  tcase_add_test(tc, test_div_knuth_divisor_two_nonzero_limbs);
  tcase_add_test(tc, test_sub_big_borrow_in_limbs);
  tcase_add_test(tc, test_mul_tiny_nonzero_product_underflows_to_zero);
  tcase_add_test(tc, test_mul_float_min_product_underflow_error2);
  tcase_add_test(tc, test_div_long_quotient_mul10_carry_stops_refinement);
  tcase_add_test(tc, test_from_float_negative_try_small);
  tcase_add_test(tc, test_from_float_minus_zero_bits);
  tcase_add_test(tc, test_from_float_sci_large_needs_downscale);
  tcase_add_test(tc, test_from_float_try_small_eight_digit_collapse);
  tcase_add_test(tc, test_cov_big_decimal_bit_shift_div_sub_null);
  tcase_add_test(tc, test_cov_big_decimal_mul10_carry);
  tcase_add_test(tc, test_cov_decimal_copy_get_bit_fractional_null);
  tcase_add_test(tc, test_cov_big_decimal_div_by_zero);
  tcase_add_test(tc, test_cov_big_decimal_div_knuth_s0_remainder_path);
  tcase_add_test(tc, test_cov_finalize_bank_round_trim_after_increment);
  tcase_add_test(tc, test_cov_from_float_bank_round_scaled_branches);
  tcase_add_test(tc, test_cov_from_float_sci_write_overflow);
  tcase_add_test(tc, test_cov_try_small_pump_and_collapse);
  tcase_add_test(tc, test_from_float_sci_very_large_positive);
  tcase_add_test(tc, test_div_tiny_scale28_by_max_returns_too_small);
  tcase_add_test(tc, test_cov_from_float_sci_parse_buffer_without_E);
  tcase_add_test(tc, test_cov_sci_div_until_fit_nonzero_last_rem_sets_tail);
  tcase_add_test(tc, test_cov_sci_bank_round_last_round_up_high_rem);
  tcase_add_test(tc, test_cov_sci_bank_round_last_half_with_tail);
  tcase_add_test(tc, test_cov_div_refine_mul10_carry_nonzero_remainder);
  tcase_add_test(tc, test_cov_try_small_mantissa_ge_ten_million_collapses);
}

/* Регистрация кейсов: функции ≤50 строк, один выход (принципы структурного
 * программирования). */

static void register_converters_int_tests(TCase* tc) {
  tcase_add_test(tc, test_from_int_to_decimal_positive);
  tcase_add_test(tc, test_from_int_to_decimal_negative);
  tcase_add_test(tc, test_from_int_to_decimal_zero);
  tcase_add_test(tc, test_from_int_to_decimal_null);
  tcase_add_test(tc, test_from_decimal_to_int_positive);
  tcase_add_test(tc, test_from_decimal_to_int_negative);
  tcase_add_test(tc, test_from_decimal_to_int_with_scale);
  tcase_add_test(tc, test_from_decimal_to_int_rounding);
  tcase_add_test(tc, test_from_decimal_to_int_overflow);
  tcase_add_test(tc, test_from_decimal_to_int_null);
  tcase_add_test(tc, test_from_decimal_to_int_int_max);
  tcase_add_test(tc, test_from_decimal_to_int_int_min);
  tcase_add_test(tc, test_from_decimal_to_int_overflow_positive);
  tcase_add_test(tc, test_from_decimal_to_int_overflow_negative);
}

static void register_converters_float_tests(TCase* tc) {
  tcase_add_test(tc, test_from_float_to_decimal_basic);
  tcase_add_test(tc, test_from_float_to_decimal_negative);
  tcase_add_test(tc, test_from_float_to_decimal_zero);
  tcase_add_test(tc, test_from_float_to_decimal_very_small);
  tcase_add_test(tc, test_from_float_to_decimal_null);
  tcase_add_test(tc, test_from_float_to_decimal_nan_inf);
  tcase_add_test(tc, test_from_float_to_decimal_overflow_positive);
  tcase_add_test(tc, test_from_float_to_decimal_overflow_negative);
  tcase_add_test(tc, test_from_decimal_to_float_positive);
  tcase_add_test(tc, test_from_decimal_to_float_negative);
  tcase_add_test(tc, test_from_decimal_to_float_zero);
  tcase_add_test(tc, test_from_decimal_to_float_high_mantissa_bit);
  tcase_add_test(tc, test_from_decimal_to_float_null);
}

static void register_arithmetic_tests_add(TCase* tc) {
  tcase_add_test(tc, test_add_basic);
  tcase_add_test(tc, test_add_negative);
  tcase_add_test(tc, test_add_with_scale);
  tcase_add_test(tc, test_add_both_negative);
  tcase_add_test(tc, test_add_zero);
  tcase_add_test(tc, test_add_nonzero_plus_zero);
  tcase_add_test(tc, test_add_positive_plus_negative);
  tcase_add_test(tc, test_add_both_negative_zeros);
  tcase_add_test(tc, test_add_null);
  tcase_add_test(tc, test_add_overflow_max_plus_two);
  tcase_add_test(tc, test_add_max_max_scale_2_fits_after_bank_round);
  tcase_add_test(tc, test_add_max_max_scale_3_fits_after_two_bank_rounds);
  tcase_add_test(tc, test_add_max_max_scale_4_fits_after_bank_round);
}

static void register_arithmetic_tests_sub(TCase* tc) {
  tcase_add_test(tc, test_sub_basic);
  tcase_add_test(tc, test_sub_negative_result);
  tcase_add_test(tc, test_sub_with_scale);
  tcase_add_test(tc, test_sub_null);
  tcase_add_test(tc, test_sub_neg_zero_minus_pos_zero);
  tcase_add_test(tc, test_sub_equal_values);
  tcase_add_test(tc, test_sub_zero_minus_value);
  tcase_add_test(tc, test_sub_second_operand_zero);
  tcase_add_test(tc, test_sub_positive_minus_negative);
  tcase_add_test(tc, test_sub_negative_minus_positive);
}

static void register_arithmetic_tests_mul(TCase* tc) {
  tcase_add_test(tc, test_mul_basic);
  tcase_add_test(tc, test_mul_with_scale);
  tcase_add_test(tc, test_mul_negative);
  tcase_add_test(tc, test_mul_null);
  tcase_add_test(tc, test_mul_overflow_positive);
  tcase_add_test(tc, test_mul_overflow_negative_sign);
  // tcase_add_test(tc, test_mul_tiny_scale_28_product_reduces_scale);
}

static void register_arithmetic_tests_div(TCase* tc) {
  tcase_add_test(tc, test_div_basic);
  tcase_add_test(tc, test_div_with_scale);
  tcase_add_test(tc, test_div_by_zero);
  tcase_add_test(tc, test_div_dividend_smaller_scale_than_divisor);
  tcase_add_test(tc, test_div_null);
  tcase_add_test(tc, test_div_max_over_one_tenth_overflow);
  tcase_add_test(
      tc, test_div_max_mantissa_over_four_tenths_overflows_with_bank_round);
  tcase_add_test(tc, test_div_rejects_scale_29_invalid_format);
}

static void register_arithmetic_tests_invalid_and_misc(TCase* tc) {
  tcase_add_test(tc, test_invalid_decimal_format_reserved_low_bits);
  tcase_add_test(tc, test_invalid_decimal_format_reserved_bits_24_30);
  tcase_add_test(tc, test_invalid_decimal_format_scale_29);
  tcase_add_test(tc, test_add_max_plus_tiny_scale28);
  tcase_add_test(tc, test_sub_max_minus_tiny_scale28);
  tcase_add_test(tc, test_is_equal_max_vs_tiny_different_scale);
  tcase_add_test(tc, test_round_half_bank_odd_integer);
  tcase_add_test(tc, test_from_float_positive_power_large);
  tcase_add_test(tc, test_from_float_boundary_min_abs_for_deep_scale);
  tcase_add_test(tc, test_from_float_d8_fifth_digit_bank_round);
  tcase_add_test(tc, test_mul_max_mantissa_overflow);
}

Suite* suite_converters(void) {
  Suite* s = suite_create("Converters");
  TCase* tc = tcase_create("Core");
  register_converters_int_tests(tc);
  register_converters_float_tests(tc);
  suite_add_tcase(s, tc);
  return s;
}

Suite* suite_comparisons(void) {
  Suite* s = suite_create("Comparisons");
  TCase* tc = tcase_create("Core");

  tcase_add_test(tc, test_is_less);
  tcase_add_test(tc, test_is_less_with_scale);
  tcase_add_test(tc, test_is_less_negative);
  tcase_add_test(tc, test_is_less_or_equal_eq);
  tcase_add_test(tc, test_is_greater);
  tcase_add_test(tc, test_is_greater_or_equal);
  tcase_add_test(tc, test_is_equal);
  tcase_add_test(tc, test_is_equal_different_scale);
  tcase_add_test(tc, test_is_equal_positive_zero_negative_zero);
  tcase_add_test(tc, test_is_not_equal);
  tcase_add_test(tc, test_is_less_opposite_signs);

  suite_add_tcase(s, tc);
  return s;
}

Suite* suite_arithmetic(void) {
  Suite* s = suite_create("Arithmetic");
  TCase* tc = tcase_create("Core");
  register_arithmetic_tests_add(tc);
  register_arithmetic_tests_sub(tc);
  register_arithmetic_tests_mul(tc);
  register_arithmetic_tests_div(tc);
  register_arithmetic_tests_invalid_and_misc(tc);
  register_coverage_gap_tests(tc);
  suite_add_tcase(s, tc);
  return s;
}

Suite* suite_other(void) {
  Suite* s = suite_create("Other Functions");
  TCase* tc = tcase_create("Core");

  tcase_add_test(tc, test_floor);
  tcase_add_test(tc, test_floor_negative);
  tcase_add_test(tc, test_round);
  tcase_add_test(tc, test_round_bank);
  tcase_add_test(tc, test_round_scale_gt_one);
  tcase_add_test(tc, test_round_negative_scale_gt_one);
  tcase_add_test(tc, test_truncate);
  tcase_add_test(tc, test_negate);
  tcase_add_test(tc, test_negate_zero);
  tcase_add_test(tc, test_floor_null);
  tcase_add_test(tc, test_round_null);
  tcase_add_test(tc, test_truncate_null);
  tcase_add_test(tc, test_negate_null);

  suite_add_tcase(s, tc);
  return s;
}

static void run_suite(SRunner* sr, Suite* s) { srunner_add_suite(sr, s); }

int main(void) {
  int exit_code = 1;
  SRunner* sr = srunner_create(suite_converters());

  run_suite(sr, suite_comparisons());
  run_suite(sr, suite_arithmetic());
  run_suite(sr, suite_other());

  srunner_run_all(sr, CK_VERBOSE);
  if (srunner_ntests_failed(sr) == 0) {
    exit_code = 0;
  }
  srunner_free(sr);
  return exit_code;
}
