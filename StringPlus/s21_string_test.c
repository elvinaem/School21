

#include "s21_string.h"

#include <check.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

START_TEST(test_memchr_found) {
  const char* s = "Hello, World!";
  ck_assert_ptr_eq(memchr(s, 'H', 13), s21_memchr(s, 'H', 13));
  ck_assert_ptr_eq(memchr(s, 'o', 13), s21_memchr(s, 'o', 13));
  ck_assert_ptr_eq(memchr(s, '!', 13), s21_memchr(s, '!', 13));
  ck_assert_ptr_eq(memchr(s, 'l', 13), s21_memchr(s, 'l', 13));
  ck_assert_ptr_eq(memchr(s, ' ', 13), s21_memchr(s, ' ', 13));
  ck_assert_ptr_eq(memchr(s, ',', 13), s21_memchr(s, ',', 13));
  ck_assert_ptr_eq(memchr(s, 'W', 13), s21_memchr(s, 'W', 13));
}
END_TEST

START_TEST(test_memchr_not_found) {
  const char* s = "Hello, World!";
  ck_assert_ptr_null(s21_memchr(s, 'z', 13));
  ck_assert_ptr_null(s21_memchr(s, 'h', 13));
  ck_assert_ptr_null(s21_memchr("", 'a', 0));
  ck_assert_ptr_null(s21_memchr("abc", 'd', 3));
}
END_TEST

START_TEST(test_memchr_limit) {
  const char* s = "abcdef";
  ck_assert_ptr_eq(memchr(s, 'd', 3), s21_memchr(s, 'd', 3));
  ck_assert_ptr_eq(memchr(s, 'd', 4), s21_memchr(s, 'd', 4));
  ck_assert_ptr_eq(memchr(s, 'a', 1), s21_memchr(s, 'a', 1));
  ck_assert_ptr_eq(memchr(s, 'b', 1), s21_memchr(s, 'b', 1));
}
END_TEST

START_TEST(test_memchr_null_char) {
  const char* s = "Hello\0World";
  ck_assert_ptr_eq(memchr(s, '\0', 11), s21_memchr(s, '\0', 11));
  ck_assert_ptr_eq(memchr(s, 'W', 11), s21_memchr(s, 'W', 11));
  ck_assert_ptr_eq(memchr(s, 'H', 11), s21_memchr(s, 'H', 11));
}
END_TEST

START_TEST(test_memchr_binary_data) {
  unsigned char data[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};
  ck_assert_ptr_eq(memchr(data, 0x34, 6), s21_memchr(data, 0x34, 6));
  ck_assert_ptr_eq(memchr(data, 0x9A, 6), s21_memchr(data, 0x9A, 6));
  ck_assert_ptr_eq(memchr(data, 0xFF, 6), s21_memchr(data, 0xFF, 6));
  ck_assert_ptr_eq(memchr(data, 0x12, 1), s21_memchr(data, 0x12, 1));
}
END_TEST

START_TEST(test_memchr_large_n) {
  char large_buffer[1024];
  memset(large_buffer, 'A', sizeof(large_buffer));
  large_buffer[500] = 'B';
  large_buffer[1000] = 'C';

  ck_assert_ptr_eq(memchr(large_buffer, 'B', 1024),
                   s21_memchr(large_buffer, 'B', 1024));
  ck_assert_ptr_eq(memchr(large_buffer, 'C', 1024),
                   s21_memchr(large_buffer, 'C', 1024));
  ck_assert_ptr_null(s21_memchr(large_buffer, 'D', 1024));
}
END_TEST

START_TEST(test_memchr_overlap) {
  char str[] = "abcabc";
  ck_assert_ptr_eq(memchr(str, 'a', 6), s21_memchr(str, 'a', 6));
  ck_assert_ptr_eq(memchr(str, 'b', 6), s21_memchr(str, 'b', 6));
  ck_assert_ptr_eq(memchr(str, 'c', 6), s21_memchr(str, 'c', 6));
}
END_TEST

START_TEST(test_memchr_performance) {
  char big[10000];
  memset(big, 'x', sizeof(big));
  big[9999] = 'y';

  void* res = s21_memchr(big, 'y', 10000);

  ck_assert_ptr_eq(res, &big[9999]);
}
END_TEST

START_TEST(test_memcmp_equal) {
  ck_assert_int_eq(s21_memcmp("AAAA", "AAAA", 4), 0);
  ck_assert_int_eq(s21_memcmp("AAAA", "AAAA", 2), 0);
  ck_assert_int_eq(s21_memcmp("AAAA", "AAAA", 0), 0);
  ck_assert_int_eq(s21_memcmp("", "", 0), 0);
  ck_assert_int_eq(s21_memcmp("\0\0\0", "\0\0\0", 3), 0);
  ck_assert_int_eq(s21_memcmp("abc", "xyz", 0), 0);
}
END_TEST

START_TEST(test_memcmp_sign) {
  int r1, r2;
  r1 = memcmp("Hello, World!", "Hello, world!", 14);
  r2 = s21_memcmp("Hello, World!", "Hello, world!", 14);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  r1 = memcmp("abc", "abd", 3);
  r2 = s21_memcmp("abc", "abd", 3);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  r1 = memcmp("abd", "abc", 3);
  r2 = s21_memcmp("abd", "abc", 3);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  r1 = memcmp("\xff\xfe\xfd", "\xff\xfe\xfe", 3);
  r2 = s21_memcmp("\xff\xfe\xfd", "\xff\xfe\xfe", 3);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));
}
END_TEST

START_TEST(test_memcmp_embedded_null) {
  int r1 = memcmp("Hello\0world", "HelloXworld", 12);
  int r2 = s21_memcmp("Hello\0world", "HelloXworld", 12);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  ck_assert_int_eq(s21_memcmp("Hello\0world", "Hello\0world", 12), 0);
}
END_TEST

START_TEST(test_memcmp_negative_values) {
  char a[] = {-1, -2, -3};
  char b[] = {-1, -2, -4};
  ck_assert_int_gt(s21_memcmp(a, b, 3), 0);

  char c[] = {-128, 0, 127};
  char d[] = {-128, 0, 126};
  ck_assert_int_gt(s21_memcmp(c, d, 3), 0);

  ck_assert_int_eq(s21_memcmp("\xff", "\x00", 1), 255);
}
END_TEST

START_TEST(test_memcmp_max_values) {
  unsigned char max1[] = {255, 255, 255};
  unsigned char max2[] = {255, 255, 254};
  ck_assert_int_gt(s21_memcmp(max1, max2, 3), 0);

  unsigned char min1[] = {0, 0, 0};
  unsigned char min2[] = {0, 0, 1};
  ck_assert_int_lt(s21_memcmp(min1, min2, 3), 0);
}
END_TEST

START_TEST(test_memcmp_different_types) {
  int int_arr1[] = {1, 2, 3, 4};
  int int_arr2[] = {1, 2, 3, 5};
  ck_assert_int_lt(s21_memcmp(int_arr1, int_arr2, sizeof(int) * 4), 0);

  float float_arr1[] = {1.0f, 2.0f, 3.0f};
  float float_arr2[] = {1.0f, 2.0f, 3.0f};
  ck_assert_int_eq(s21_memcmp(float_arr1, float_arr2, sizeof(float) * 3), 0);

  double double_arr1[] = {1.0, 2.0};
  double double_arr2[] = {1.0, 2.1};
  ck_assert_int_lt(s21_memcmp(double_arr1, double_arr2, sizeof(double) * 2), 0);
}
END_TEST

START_TEST(test_memcmp_null_ptrs) {
  char* ptr1 = NULL;
  char* ptr2 = NULL;
  ck_assert_int_eq(s21_memcmp(ptr1, ptr2, 0), 0);
  ck_assert_int_eq(s21_memcmp(ptr1, "test", 0), 0);
  ck_assert_int_eq(s21_memcmp("test", ptr2, 0), 0);
}
END_TEST

START_TEST(test_memcmp_large_n) {
  char big1[1024];
  char big2[1024];

  memset(big1, 'A', 1024);
  memset(big2, 'A', 1024);
  big2[1000] = 'B';

  ck_assert_int_eq(s21_memcmp(big1, big2, 1024), -1);
  ck_assert_int_eq(s21_memcmp(big2, big1, 1024), 1);
  ck_assert_int_eq(s21_memcmp(big1, big1, 1024), 0);
}
END_TEST

START_TEST(test_memcmp_early_exit) {
  char a[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char b[] = "ABCDEFGHIJKLMNOPQRSTUVWXYz";

  int result = s21_memcmp(a, b, 26);

  ck_assert_int_eq(result, 'Z' - 'z');
}
END_TEST

START_TEST(test_memcmp_volatile) {
  volatile char varr1[] = {10, 20, 30, 40, 50};
  volatile char varr2[] = {10, 20, 30, 40, 50};

  ck_assert_int_eq(s21_memcmp((void*)varr1, (void*)varr2, 5), 0);

  varr2[3] = 41;
  ck_assert_int_lt(s21_memcmp((void*)varr1, (void*)varr2, 5), 0);
}
END_TEST

START_TEST(test_memcmp_repeating_patterns) {
  char pattern1[32];
  char pattern2[32];

  memset(pattern1, 0xAA, 32);
  memset(pattern2, 0xAA, 32);
  ck_assert_int_eq(s21_memcmp(pattern1, pattern2, 32), 0);

  memset(pattern2, 0xAB, 32);
  ck_assert_int_lt(s21_memcmp(pattern1, pattern2, 32), 0);
}
END_TEST

START_TEST(test_memcpy_basic) {
  char d1[64], d2[64];

  memcpy(d1, "Hello, World!", 14);
  s21_memcpy(d2, "Hello, World!", 14);
  ck_assert_mem_eq(d1, d2, 14);

  memcpy(d1, "abc", 4);
  s21_memcpy(d2, "abc", 4);
  ck_assert_mem_eq(d1, d2, 4);

  memcpy(d1, "", 1);
  s21_memcpy(d2, "", 1);
  ck_assert_mem_eq(d1, d2, 1);
}
END_TEST

START_TEST(test_memcpy_bytes) {
  char d1[16], d2[16];
  const char src[] = "\xff\xfe\xfd\x00\x01";

  memcpy(d1, src, 5);
  s21_memcpy(d2, src, 5);
  ck_assert_mem_eq(d1, d2, 5);

  memcpy(d1, "Test", 0);
  s21_memcpy(d2, "Test", 0);

  memcpy(d1, "1234567890", 10);
  s21_memcpy(d2, "1234567890", 10);
  ck_assert_mem_eq(d1, d2, 10);
}
END_TEST

START_TEST(test_memcpy_overlap) {
  char buffer[32];

  memset(buffer, 'A', 32);

  char* src = buffer + 2;
  char* dest = buffer + 5;

  s21_memcpy(dest, src, 10);

  for (int i = 0; i < 10; i++) {
    ck_assert_int_eq(dest[i], src[i]);
  }

  src = buffer + 10;
  dest = buffer + 3;

  s21_memcpy(dest, src, 8);

  for (int i = 0; i < 8; i++) {
    ck_assert_int_eq(dest[i], src[i]);
  }
}
END_TEST

START_TEST(test_memcpy_large) {
  char src[8192];
  char dest1[8192];
  char dest2[8192];

  for (int i = 0; i < 8192; i++) {
    src[i] = (char)(i % 256);
  }

  memcpy(dest1, src, 8192);
  s21_memcpy(dest2, src, 8192);
  ck_assert_mem_eq(dest1, dest2, 8192);

  memcpy(dest1, src + 1000, 500);
  s21_memcpy(dest2, src + 1000, 500);
  ck_assert_mem_eq(dest1, dest2, 500);
}
END_TEST

START_TEST(test_memcpy_alignment) {
  char src[64] = {0};
  char dest1[64] = {0};
  char dest2[64] = {0};

  for (int i = 0; i < 64; i++) {
    src[i] = (char)(i + 1);
  }

  for (int dest_offset = 0; dest_offset < 8; dest_offset++) {
    for (int src_offset = 0; src_offset < 8; src_offset++) {
      memset(dest1, 0, 64);
      memset(dest2, 0, 64);

      char* dest_ptr1 = dest1 + dest_offset;
      char* dest_ptr2 = dest2 + dest_offset;
      char* src_ptr = src + src_offset;

      int copy_size =
          32 - (dest_offset > src_offset ? dest_offset : src_offset);
      if (copy_size > 0) {
        memcpy(dest_ptr1, src_ptr, copy_size);
        s21_memcpy(dest_ptr2, src_ptr, copy_size);
        ck_assert_mem_eq(dest_ptr1, dest_ptr2, copy_size);
      }
    }
  }
}
END_TEST

START_TEST(test_memcpy_boundary) {
  char src[10] = "123456789";
  char dest1[10] = {0};
  char dest2[10] = {0};

  s21_memcpy(dest1, src, 0);
  ck_assert_mem_eq(dest1, dest2, 10);

  memcpy(dest1, src, 1);
  s21_memcpy(dest2, src, 1);
  ck_assert_mem_eq(dest1, dest2, 1);

  memcpy(dest1, src, 9);
  s21_memcpy(dest2, src, 9);
  ck_assert_mem_eq(dest1, dest2, 9);

  char large_src[65536];
  char large_dest1[65536];
  char large_dest2[65536];

  for (int i = 0; i < 65536; i++) {
    large_src[i] = (char)(i % 256);
  }

  memcpy(large_dest1, large_src, 65536);
  s21_memcpy(large_dest2, large_src, 65536);
  ck_assert_mem_eq(large_dest1, large_dest2, 65536);
}
END_TEST

START_TEST(test_memcpy_null_ptrs) {
  char dest[10];
  s21_memcpy(dest, NULL, 0);
  s21_memcpy(NULL, "test", 0);
  s21_memcpy(NULL, NULL, 0);
  ck_assert_mem_eq(dest, dest, 10);
}
END_TEST

START_TEST(test_memcpy_different_types) {
  int src_int[] = {1, 2, 3, 4, 5};
  int dest_int1[5];
  int dest_int2[5];

  memcpy(dest_int1, src_int, sizeof(src_int));
  s21_memcpy(dest_int2, src_int, sizeof(src_int));
  ck_assert_mem_eq(dest_int1, dest_int2, sizeof(src_int));

  float src_float[] = {1.1f, 2.2f, 3.3f, 4.4f, 5.5f};
  float dest_float1[5];
  float dest_float2[5];

  memcpy(dest_float1, src_float, sizeof(src_float));
  s21_memcpy(dest_float2, src_float, sizeof(src_float));
  ck_assert_mem_eq(dest_float1, dest_float2, sizeof(src_float));

  double src_double[] = {1.11, 2.22, 3.33, 4.44, 5.55};
  double dest_double1[5];
  double dest_double2[5];

  memcpy(dest_double1, src_double, sizeof(src_double));
  s21_memcpy(dest_double2, src_double, sizeof(src_double));
  ck_assert_mem_eq(dest_double1, dest_double2, sizeof(src_double));

  struct test_struct {
    int a;
    float b;
    char c[10];
  } src_struct = {42, 3.14f, "Hello"}, dest_struct1, dest_struct2;

  memcpy(&dest_struct1, &src_struct, sizeof(src_struct));
  s21_memcpy(&dest_struct2, &src_struct, sizeof(src_struct));
  ck_assert_mem_eq(&dest_struct1, &dest_struct2, sizeof(src_struct));
}
END_TEST

START_TEST(test_memset_large) {
  char d1[8192];
  char d2[8192];

  memset(d1, 'B', 8192);
  s21_memset(d2, 'B', 8192);
  ck_assert_mem_eq(d1, d2, 8192);

  memset(d1, 0xAA, 5000);
  s21_memset(d2, 0xAA, 5000);
  ck_assert_mem_eq(d1, d2, 5000);

  memset(d1, 0, 8192);
  s21_memset(d2, 0, 8192);
  ck_assert_mem_eq(d1, d2, 8192);
}
END_TEST

START_TEST(test_memset_alignment) {
  char buffer1[64];
  char buffer2[64];

  for (int offset = 0; offset < 8; offset++) {
    char* ptr1 = buffer1 + offset;
    char* ptr2 = buffer2 + offset;

    memset(buffer1, 0, 64);
    memset(buffer2, 0, 64);

    int fill_size = 32;

    memset(ptr1, 0x55, fill_size);
    s21_memset(ptr2, 0x55, fill_size);

    ck_assert_mem_eq(ptr1, ptr2, fill_size);

    for (int i = 0; i < offset; i++) {
      ck_assert_int_eq(buffer1[i], 0);
      ck_assert_int_eq(buffer2[i], 0);
    }
  }
}
END_TEST

START_TEST(test_memset_edge_values) {
  char d1[16];
  char d2[16];

  memset(d1, -1, 16);
  s21_memset(d2, -1, 16);
  ck_assert_mem_eq(d1, d2, 16);

  memset(d1, 300, 16);
  s21_memset(d2, 300, 16);
  ck_assert_mem_eq(d1, d2, 16);

  memset(d1, 2147483647, 16);
  s21_memset(d2, 2147483647, 16);
  ck_assert_mem_eq(d1, d2, 16);

  memset(d1, -2147483648, 16);
  s21_memset(d2, -2147483648, 16);
  ck_assert_mem_eq(d1, d2, 16);
}
END_TEST

START_TEST(test_memset_null_ptr) {
  char dest[10] = "123456789";
  char original[10];
  memcpy(original, dest, 10);

  void* result = s21_memset(NULL, 'A', 0);
  ck_assert_ptr_null(result);

  result = s21_memset(dest, 'B', 0);
  ck_assert_ptr_eq(result, dest);
  ck_assert_mem_eq(dest, original, 10);

  char* ret = s21_memset(dest, 'X', 5);
  ck_assert_ptr_eq(ret, dest);
}
END_TEST

START_TEST(test_memset_partial) {
  char d1[32];
  char d2[32];

  for (int i = 0; i < 32; i++) {
    d1[i] = d2[i] = (char)i;
  }

  memset(d1, 'X', 5);
  s21_memset(d2, 'X', 5);
  ck_assert_mem_eq(d1, d2, 5);
  ck_assert_mem_eq(d1 + 5, d2 + 5, 27);

  memset(d1 + 10, 'Y', 8);
  s21_memset(d2 + 10, 'Y', 8);
  ck_assert_mem_eq(d1 + 10, d2 + 10, 8);
  ck_assert_mem_eq(d1, d2, 32);

  memset(d1 + 20, 'Z', 12);
  s21_memset(d2 + 20, 'Z', 12);
  ck_assert_mem_eq(d1 + 20, d2 + 20, 12);
  ck_assert_mem_eq(d1, d2, 32);
}
END_TEST

START_TEST(test_memset_max_size) {
  char* d1 = malloc(1000000);
  char* d2 = malloc(1000000);

  if (d1 && d2) {
    memset(d1, 'M', 1000000);
    s21_memset(d2, 'M', 1000000);
    ck_assert_mem_eq(d1, d2, 1000000);

    memset(d1, 0, 1000000);
    s21_memset(d2, 0, 1000000);
    ck_assert_mem_eq(d1, d2, 1000000);
  }

  free(d1);
  free(d2);
}
END_TEST

START_TEST(test_memset_multiple_calls) {
  char d1[32];
  char d2[32];
  memset(d1, 'A', 10);
  memset(d1 + 10, 'B', 10);
  memset(d1 + 20, 'C', 12);

  s21_memset(d2, 'A', 10);
  s21_memset(d2 + 10, 'B', 10);
  s21_memset(d2 + 20, 'C', 12);

  ck_assert_mem_eq(d1, d2, 32);
}
END_TEST

START_TEST(test_memset_basic) {
  char d1[64] = {0}, d2[64] = {0};

  memset(d1, 'A', 14);
  s21_memset(d2, 'A', 14);
  ck_assert_mem_eq(d1, d2, 64);

  memset(d1, 0, 3);
  s21_memset(d2, 0, 3);
  ck_assert_mem_eq(d1, d2, 64);
}
END_TEST

START_TEST(test_memset_values) {
  char d1[32], d2[32];

  memset(d1, 0xFF, 20);
  s21_memset(d2, 0xFF, 20);
  ck_assert_mem_eq(d1, d2, 20);

  memset(d1, '\0', 8);
  s21_memset(d2, '\0', 8);
  ck_assert_mem_eq(d1, d2, 8);

  memset(d1, 255, 4);
  s21_memset(d2, 255, 4);
  ck_assert_mem_eq(d1, d2, 4);
}
END_TEST

START_TEST(test_strncat_basic) {
  char d1[128], d2[128];

  strcpy(d1, "AAA");
  strcpy(d2, "AAA");
  strncat(d1, "Hello", 3);
  s21_strncat(d2, "Hello", 3);
  ck_assert_str_eq(d1, d2);

  strcpy(d1, "AAA");
  strcpy(d2, "AAA");
  strncat(d1, "Hello", 0);
  s21_strncat(d2, "Hello", 0);
  ck_assert_str_eq(d1, d2);

  strcpy(d1, "AAA");
  strcpy(d2, "AAA");
  strncat(d1, "", 5);
  s21_strncat(d2, "", 5);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_trunc) {
  char d1[128], d2[128];

  strcpy(d1, "abc");
  strcpy(d2, "abc");
  strncat(d1, "def", 2);
  s21_strncat(d2, "def", 2);
  ck_assert_str_eq(d1, d2);

  strcpy(d1, "ABC");
  strcpy(d2, "ABC");
  strncat(d1, "XYZ", 0);
  s21_strncat(d2, "XYZ", 0);
  ck_assert_str_eq(d1, d2);

  strcpy(d1, "short");
  strcpy(d2, "short");
  strncat(d1, "longer string", 4);
  s21_strncat(d2, "longer string", 4);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_null_terminator) {
  char d1[128] = "Hello";
  char d2[128] = "Hello";

  char src[] = "World\0Hidden";

  strncat(d1, src, 10);
  s21_strncat(d2, src, 10);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_empty_dest) {
  char d1[128] = "";
  char d2[128] = "";

  strncat(d1, "World", 3);
  s21_strncat(d2, "World", 3);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_n_larger_than_src) {
  char d1[128] = "Start";
  char d2[128] = "Start";

  strncat(d1, "End", 10);
  s21_strncat(d2, "End", 10);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_exact_fit) {
  char d1[128] = "Prefix";
  char d2[128] = "Prefix";
  char src[] = "Suffix";

  strncat(d1, src, strlen(src));
  s21_strncat(d2, src, strlen(src));
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_partial_copy) {
  char d1[128] = "123";
  char d2[128] = "123";

  strncat(d1, "456789", 3);
  s21_strncat(d2, "456789", 3);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_boundary_values) {
  char d1[128] = "A";
  char d2[128] = "A";

  strncat(d1, "Test", 0);
  s21_strncat(d2, "Test", 0);
  ck_assert_str_eq(d1, d2);

  strcpy(d1, "X");
  strcpy(d2, "X");
  strncat(d1, "YZ", 1);
  s21_strncat(d2, "YZ", 1);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strncat_special_chars) {
  char d1[128] = "Start";
  char d2[128] = "Start";

  char src[] = "\t\n\r\v\f";

  strncat(d1, src, 5);
  s21_strncat(d2, src, 5);
  ck_assert_mem_eq(d1, d2, strlen(d1) + 1);
}
END_TEST

START_TEST(test_strncat_overlap) {
  char buffer[128] = "HelloWorld";

  char* dest = buffer + 5;
  char* src = buffer;

  s21_strncat(dest, src, 3);
}
END_TEST

START_TEST(test_strncat_large_n) {
  char d1[1024] = "Start";
  char d2[1024] = "Start";

  char large_src[512];
  for (int i = 0; i < 511; i++) {
    large_src[i] = 'A' + (i % 26);
  }
  large_src[511] = '\0';

  strncat(d1, large_src, 500);
  s21_strncat(d2, large_src, 500);
  ck_assert_str_eq(d1, d2);
}
END_TEST

START_TEST(test_strchr_basic) {
  const char* s = "Hello, World!";
  ck_assert_ptr_eq(strchr(s, 'H'), s21_strchr(s, 'H'));
  ck_assert_ptr_eq(strchr(s, 'o'), s21_strchr(s, 'o'));
  ck_assert_ptr_eq(strchr(s, '!'), s21_strchr(s, '!'));
  ck_assert_ptr_eq(strchr(s, '\0'), s21_strchr(s, '\0'));
  ck_assert_ptr_eq(strchr(s, 'z'), s21_strchr(s, 'z'));
  ck_assert_ptr_eq(strchr(s, 'l'), s21_strchr(s, 'l'));
  ck_assert_ptr_eq(strchr(s, ' '), s21_strchr(s, ' '));
}
END_TEST

START_TEST(test_strchr_edge) {
  const char* s1 = "Hello123";
  const char* s2 = "!@#$%^&*()";
  const char* s3 = "abc";
  const char* s4 = "";
  ck_assert_ptr_eq(strchr(s1, '2'), s21_strchr(s1, '2'));
  ck_assert_ptr_eq(strchr(s2, '^'), s21_strchr(s2, '^'));
  ck_assert_ptr_eq(strchr(s3, -1), s21_strchr(s3, -1));
  ck_assert_ptr_eq(strchr(s4, 'a'), s21_strchr(s4, 'a'));
  ck_assert_ptr_eq(strchr(s4, '\0'), s21_strchr(s4, '\0'));
  ck_assert_ptr_eq(strchr(s3, 'c'), s21_strchr(s3, 'c'));
  ck_assert_ptr_eq(strchr(s3, 127), s21_strchr(s3, 127));
}
END_TEST

START_TEST(test_strchr_first_occurrence) {
  const char* s1 = "aba";
  ck_assert_ptr_eq(strchr(s1, 'a'), s21_strchr(s1, 'a'));

  const char* s2 = "X";
  ck_assert_ptr_eq(strchr(s2, 'X'), s21_strchr(s2, 'X'));
}
END_TEST

START_TEST(test_strchr_null_char_only) {
  const char* s = "";
  ck_assert_ptr_eq(strchr(s, '\0'), s21_strchr(s, '\0'));
  ck_assert_ptr_eq(s21_strchr(s, '\0'), s);
}
END_TEST

START_TEST(test_strchr_not_found) {
  const char* s = "Hello World";
  ck_assert_ptr_null(s21_strchr(s, 'z'));
  ck_assert_ptr_null(s21_strchr(s, 'x'));
  ck_assert_ptr_null(s21_strchr(s, 'Z'));
  ck_assert_ptr_null(s21_strchr(s, 128));
}
END_TEST

START_TEST(test_strchr_empty_string) {
  const char* s = "";
  ck_assert_ptr_null(s21_strchr(s, 'a'));
  ck_assert_ptr_eq(strchr(s, '\0'), s21_strchr(s, '\0'));
}
END_TEST

START_TEST(test_strchr_single_char_string) {
  const char* s = "X";
  ck_assert_ptr_eq(strchr(s, 'X'), s21_strchr(s, 'X'));
  ck_assert_ptr_null(s21_strchr(s, 'Y'));
  ck_assert_ptr_eq(strchr(s, '\0'), s21_strchr(s, '\0'));
}
END_TEST

START_TEST(test_strchr_all_chars_same) {
  const char* s = "aaaaa";
  ck_assert_ptr_eq(strchr(s, 'a'), s21_strchr(s, 'a'));
  ck_assert_ptr_null(s21_strchr(s, 'b'));
}
END_TEST

START_TEST(test_strchr_negative_values) {
  const char* s = "Hello\x80World";
  ck_assert_ptr_eq(strchr(s, '\x80'), s21_strchr(s, '\x80'));

  ck_assert_ptr_eq(strchr(s, -128), s21_strchr(s, -128));
}
END_TEST

START_TEST(test_strchr_max_values) {
  const char* s = "Hello\xFFWorld";
  ck_assert_ptr_eq(strchr(s, '\xFF'), s21_strchr(s, '\xFF'));

  ck_assert_ptr_eq(strchr(s, 255), s21_strchr(s, 255));
}
END_TEST

START_TEST(test_strchr_long_string) {
  char long_str[10000];
  for (int i = 0; i < 9998; i++) {
    long_str[i] = 'a' + (i % 26);
  }
  long_str[9998] = 'X';
  long_str[9999] = '\0';

  ck_assert_ptr_eq(strchr(long_str, 'X'), s21_strchr(long_str, 'X'));
  ck_assert_ptr_eq(strchr(long_str, long_str[0]),
                   s21_strchr(long_str, long_str[0]));
  ck_assert_ptr_eq(strchr(long_str, '\0'), s21_strchr(long_str, '\0'));
}
END_TEST

START_TEST(test_strchr_null_ptr) {
  char dest[10] = "Test";

  char* result = s21_strchr(dest, 'e');
  ck_assert_ptr_eq(result, dest + 1);
}
END_TEST

START_TEST(test_strchr_special_chars) {
  const char* s = "\t\n\r\v\f\0Hidden";

  ck_assert_ptr_eq(strchr(s, '\t'), s21_strchr(s, '\t'));
  ck_assert_ptr_eq(strchr(s, '\n'), s21_strchr(s, '\n'));
  ck_assert_ptr_eq(strchr(s, '\r'), s21_strchr(s, '\r'));
  ck_assert_ptr_eq(strchr(s, '\v'), s21_strchr(s, '\v'));
  ck_assert_ptr_eq(strchr(s, '\f'), s21_strchr(s, '\f'));
  ck_assert_ptr_eq(strchr(s, '\0'), s21_strchr(s, '\0'));
}
END_TEST

START_TEST(test_strchr_multiple_occurrences) {
  const char* s = "abacadabra";

  ck_assert_ptr_eq(strchr(s, 'a'), s21_strchr(s, 'a'));
  ck_assert_ptr_eq(strchr(s, 'b'), s21_strchr(s, 'b'));
  ck_assert_ptr_eq(strchr(s, 'c'), s21_strchr(s, 'c'));
  ck_assert_ptr_eq(strchr(s, 'd'), s21_strchr(s, 'd'));
  ck_assert_ptr_eq(strchr(s, 'r'), s21_strchr(s, 'r'));
}
END_TEST

START_TEST(test_strchr_unicode) {
  const char* s = "Hello 世界 World";

  unsigned char first_byte_of_shijie = 0xE4;

  ck_assert_ptr_eq(strchr(s, first_byte_of_shijie),
                   s21_strchr(s, first_byte_of_shijie));
}
END_TEST

START_TEST(test_strncmp_equal) {
  ck_assert_int_eq(s21_strncmp("Hello", "Hello", 5), 0);
  ck_assert_int_eq(s21_strncmp("Hello", "Hello", 3), 0);
  ck_assert_int_eq(s21_strncmp("", "", 5), 0);
  ck_assert_int_eq(s21_strncmp("Hello", "", 0), 0);
  ck_assert_int_eq(s21_strncmp("Hello", "World", 0), 0);
  ck_assert_int_eq(s21_strncmp("Hello123", "Hello123", 8), 0);
  ck_assert_int_eq(s21_strncmp("!@#$", "!@#$", 4), 0);
}
END_TEST

START_TEST(test_strncmp_sign) {
  int r1, r2;

  r1 = strncmp("Hello", "hello", 5);
  r2 = s21_strncmp("Hello", "hello", 5);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  r1 = strncmp("hello", "Hello", 5);
  r2 = s21_strncmp("hello", "Hello", 5);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  r1 = strncmp("abc", "xyz", 3);
  r2 = s21_strncmp("abc", "xyz", 3);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));

  r1 = strncmp("xyz", "abc", 3);
  r2 = s21_strncmp("xyz", "abc", 3);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));
}
END_TEST

START_TEST(test_strncmp_prefix) {
  int r1, r2;

  r1 = strncmp("Hello", "HelloWorld", 5);
  r2 = s21_strncmp("Hello", "HelloWorld", 5);
  ck_assert_int_eq(r1, r2);

  r1 = strncmp("HelloWorld", "Hello", 5);
  r2 = s21_strncmp("HelloWorld", "Hello", 5);
  ck_assert_int_eq(r1, r2);

  r1 = strncmp("HelloWorld", "HelloTest", 5);
  r2 = s21_strncmp("HelloWorld", "HelloTest", 5);
  ck_assert_int_eq(r1, r2);

  r1 = strncmp("Short", "ShortString", 5);
  r2 = s21_strncmp("Short", "ShortString", 5);
  ck_assert_int_eq(r1, r2);
}
END_TEST

START_TEST(test_strncmp_different_at_start) {
  ck_assert_int_lt(s21_strncmp("abc", "xbc", 3), 0);
  ck_assert_int_gt(s21_strncmp("xbc", "abc", 3), 0);
}
END_TEST

START_TEST(test_strncmp_different_at_end) {
  ck_assert_int_lt(s21_strncmp("abc", "abd", 3), 0);
  ck_assert_int_gt(s21_strncmp("abd", "abc", 3), 0);
}
END_TEST

START_TEST(test_strncmp_with_null_chars) {
  const char str1[] = "Hello\0World";
  const char str2[] = "Hello\0World";

  ck_assert_int_eq(s21_strncmp(str1, str2, 12), 0);

  const char str3[] = "Hello\0World";
  const char str4[] = "Hello\0Universe";

  int r1 = strncmp(str3, str4, 12);
  int r2 = s21_strncmp(str3, str4, 12);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));
}
END_TEST

START_TEST(test_strncmp_n_zero) {
  ck_assert_int_eq(s21_strncmp("Hello", "World", 0), 0);
  ck_assert_int_eq(s21_strncmp("", "", 0), 0);
  ck_assert_int_eq(s21_strncmp("Any", "Strings", 0), 0);
}
END_TEST

START_TEST(test_strncmp_n_larger_than_strings) {
  ck_assert_int_eq(s21_strncmp("abc", "abc", 10), 0);
  ck_assert_int_lt(s21_strncmp("abc", "abd", 10), 0);
}
END_TEST

START_TEST(test_strncmp_case_sensitive) {
  ck_assert_int_lt(s21_strncmp("Hello", "hello", 5), 0);
  ck_assert_int_gt(s21_strncmp("hello", "Hello", 5), 0);
}
END_TEST

START_TEST(test_strncmp_unsigned_char_values) {
  const char str1[] = {0xFF, 0x00, 0x00};
  const char str2[] = {0xFE, 0x00, 0x00};

  int r1 = strncmp(str1, str2, 1);
  int r2 = s21_strncmp(str1, str2, 1);
  ck_assert_int_eq((r1 > 0) - (r1 < 0), (r2 > 0) - (r2 < 0));
}
END_TEST

START_TEST(test_strncmp_different_at_end_with_n_limit) {
  ck_assert_int_eq(s21_strncmp("abcX", "abcY", 3), 0);
  ck_assert_int_eq(s21_strncmp("abcX", "abcY", 4), 'X' - 'Y');
}
END_TEST

START_TEST(test_strncmp_null_ptrs) {
  ck_assert_int_eq(s21_strncmp(NULL, "test", 0), 0);
  ck_assert_int_eq(s21_strncmp("test", NULL, 0), 0);
  ck_assert_int_eq(s21_strncmp(NULL, NULL, 0), 0);
}
END_TEST

START_TEST(test_strncpy_truncate) {
  char d1[64], d2[64];

  strncpy(d1, "abc\0def", 7);
  s21_strncpy(d2, "abc\0def", 7);
  ck_assert_mem_eq(d1, d2, 7);
}
END_TEST

START_TEST(test_strncpy_exact_length) {
  char d1[64] = {0};
  char d2[64] = {0};

  strncpy(d1, "Hello", 6);
  s21_strncpy(d2, "Hello", 6);

  ck_assert_mem_eq(d1, d2, 6);
}
END_TEST

START_TEST(test_strncpy_null_in_src) {
  char d1[64] = {0};
  char d2[64] = {0};
  char src[] = "Hello\0World";

  strncpy(d1, src, 10);
  s21_strncpy(d2, src, 10);

  ck_assert_mem_eq(d1, d2, 6);
}
END_TEST

START_TEST(test_strncpy_dest_larger_than_src) {
  char d1[64];
  char d2[64];

  memset(d1, 'X', 64);
  memset(d2, 'X', 64);

  strncpy(d1, "Hi", 10);
  s21_strncpy(d2, "Hi", 10);

  ck_assert_mem_eq(d1, d2, 2);
  ck_assert_int_eq(d2[2], '\0');
}
END_TEST

START_TEST(test_strncpy_src_empty) {
  char d1[64] = "NotEmpty";
  char d2[64] = "NotEmpty";
  char original[64];
  strcpy(original, d1);

  strncpy(d1, "", 5);
  s21_strncpy(d2, "", 5);

  ck_assert_int_eq(d2[0], '\0');
}
END_TEST

START_TEST(test_strncpy_special_chars) {
  char d1[64];
  char d2[64];

  char src[] = "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A";

  strncpy(d1, src, 10);
  s21_strncpy(d2, src, 10);

  ck_assert_mem_eq(d1, d2, 10);
}
END_TEST

START_TEST(test_strncpy_large_n) {
  char d1[1024];
  char d2[1024];

  char src[] = "Small string";

  memset(d1, 'X', 1024);
  memset(d2, 'X', 1024);

  strncpy(d1, src, 1000);
  s21_strncpy(d2, src, 1000);

  ck_assert_mem_eq(d1, d2, strlen(src));
  ck_assert_int_eq(d2[strlen(src)], '\0');
}
END_TEST

START_TEST(test_strncpy_return_value) {
  char dest[64] = "Test";
  char* result = s21_strncpy(dest, "New", 3);

  ck_assert_ptr_eq(result, dest);
}
END_TEST

START_TEST(test_strcspn_basic) {
  ck_assert_uint_eq(strcspn("", "abc"), s21_strcspn("", "abc"));
  ck_assert_uint_eq(strcspn("Hello World", ""), s21_strcspn("Hello World", ""));
  ck_assert_uint_eq(strcspn("", ""), s21_strcspn("", ""));
  ck_assert_uint_eq(strcspn("Hello World", "xyz"),
                    s21_strcspn("Hello World", "xyz"));
  ck_assert_uint_eq(strcspn("abcde", "a"), s21_strcspn("abcde", "a"));
  ck_assert_uint_eq(strcspn("abcde", "e"), s21_strcspn("abcde", "e"));
}
END_TEST

START_TEST(test_strcspn_chars) {
  ck_assert_uint_eq(strcspn("Hello World", "o"),
                    s21_strcspn("Hello World", "o"));
  ck_assert_uint_eq(strcspn("Hello World", "loW"),
                    s21_strcspn("Hello World", "loW"));
  ck_assert_uint_eq(strcspn("Hello World", "Wrd"),
                    s21_strcspn("Hello World", "Wrd"));
  ck_assert_uint_eq(strcspn("abc123def456", "0123456789"),
                    s21_strcspn("abc123def456", "0123456789"));
  ck_assert_uint_eq(strcspn("Hello!@# World", "!@#"),
                    s21_strcspn("Hello!@# World", "!@#"));
  ck_assert_uint_eq(strcspn("aaaaa", "a"), s21_strcspn("aaaaa", "a"));
}
END_TEST

START_TEST(test_strcspn_edge) {
  ck_assert_uint_eq(strcspn("Hello World", " \t\n"),
                    s21_strcspn("Hello World", " \t\n"));
  ck_assert_uint_eq(strcspn("Hello World", "ll"),
                    s21_strcspn("Hello World", "ll"));
  ck_assert_uint_eq(strcspn("abcde", "abc"), s21_strcspn("abcde", "abc"));
  ck_assert_uint_eq(strcspn("abcde", "cde"), s21_strcspn("abcde", "cde"));
  ck_assert_uint_eq(strcspn("!@#$%^&*()", "!@#$"),
                    s21_strcspn("!@#$%^&*()", "!@#$"));
}
END_TEST

START_TEST(test_strcspn_not_found) {
  ck_assert_uint_eq(strcspn("Hello", "xyz"), s21_strcspn("Hello", "xyz"));
  ck_assert_uint_eq(strcspn("ABC", "abc"), s21_strcspn("ABC", "abc"));
  ck_assert_uint_eq(strcspn("123", "abc"), s21_strcspn("123", "abc"));
  ck_assert_uint_eq(strcspn("!@#", "abc"), s21_strcspn("!@#", "abc"));
}
END_TEST

START_TEST(test_strcspn_found_at_start) {
  ck_assert_uint_eq(strcspn("Hello", "H"), s21_strcspn("Hello", "H"));
  ck_assert_uint_eq(strcspn("abcde", "a"), s21_strcspn("abcde", "a"));
  ck_assert_uint_eq(strcspn("12345", "1"), s21_strcspn("12345", "1"));
  ck_assert_uint_eq(strcspn("!@#$%", "!"), s21_strcspn("!@#$%", "!"));
}
END_TEST

START_TEST(test_strcspn_found_at_end) {
  ck_assert_uint_eq(strcspn("Hello", "o"), s21_strcspn("Hello", "o"));
  ck_assert_uint_eq(strcspn("abcde", "e"), s21_strcspn("abcde", "e"));
  ck_assert_uint_eq(strcspn("12345", "5"), s21_strcspn("12345", "5"));
  ck_assert_uint_eq(strcspn("!@#$%", "%"), s21_strcspn("!@#$%", "%"));
}
END_TEST

START_TEST(test_strcspn_found_in_middle) {
  ck_assert_uint_eq(strcspn("Hello", "l"), s21_strcspn("Hello", "l"));
  ck_assert_uint_eq(strcspn("abcde", "c"), s21_strcspn("abcde", "c"));
  ck_assert_uint_eq(strcspn("12345", "3"), s21_strcspn("12345", "3"));
  ck_assert_uint_eq(strcspn("!@#$%", "#"), s21_strcspn("!@#$%", "#"));
}
END_TEST

START_TEST(test_strcspn_multiple_chars_in_str2) {
  ck_assert_uint_eq(strcspn("Hello", "lx"), s21_strcspn("Hello", "lx"));
  ck_assert_uint_eq(strcspn("abcde", "cde"), s21_strcspn("abcde", "cde"));
  ck_assert_uint_eq(strcspn("12345", "345"), s21_strcspn("12345", "345"));
}
END_TEST

START_TEST(test_strcspn_special_chars) {
  ck_assert_uint_eq(strcspn("\t\n\r\v\f", "\n"),
                    s21_strcspn("\t\n\r\v\f", "\n"));
  ck_assert_uint_eq(strcspn("Hello\tWorld", "\t"),
                    s21_strcspn("Hello\tWorld", "\t"));
  ck_assert_uint_eq(strcspn("Hello\nWorld", "\n"),
                    s21_strcspn("Hello\nWorld", "\n"));
}
END_TEST

START_TEST(test_strcspn_with_null_chars) {
  const char str1[] = "Hello\0World";
  const char str2[] = "o";

  ck_assert_uint_eq(strcspn(str1, str2), s21_strcspn(str1, str2));
}
END_TEST

START_TEST(test_strcspn_empty_str1) {
  ck_assert_uint_eq(strcspn("", "abc"), s21_strcspn("", "abc"));
  ck_assert_uint_eq(strcspn("", ""), s21_strcspn("", ""));
  ck_assert_uint_eq(strcspn("", "xyz"), s21_strcspn("", "xyz"));
}
END_TEST

START_TEST(test_strcspn_empty_str2) {
  ck_assert_uint_eq(strcspn("Hello", ""), s21_strcspn("Hello", ""));
  ck_assert_uint_eq(strcspn("abcde", ""), s21_strcspn("abcde", ""));
  ck_assert_uint_eq(strcspn("12345", ""), s21_strcspn("12345", ""));
  ck_assert_uint_eq(strcspn("", ""), s21_strcspn("", ""));
}
END_TEST

START_TEST(test_strcspn_both_empty) {
  ck_assert_uint_eq(strcspn("", ""), s21_strcspn("", ""));
}
END_TEST

START_TEST(test_strcspn_long_strings) {
  char str1[1000];
  char str2[100];

  for (int i = 0; i < 999; i++) {
    str1[i] = 'a' + (i % 26);
  }
  str1[999] = '\0';

  for (int i = 0; i < 99; i++) {
    str2[i] = 'A' + (i % 26);
  }
  str2[99] = '\0';

  ck_assert_uint_eq(strcspn(str1, str2), s21_strcspn(str1, str2));

  str1[900] = 'X';
  str2[50] = 'X';
  ck_assert_uint_eq(strcspn(str1, str2), s21_strcspn(str1, str2));
}
END_TEST

START_TEST(test_strcspn_return_type) {
  s21_size_t result = s21_strcspn("Hello", "o");
  ck_assert_uint_eq(result, 4);

  result = s21_strcspn("Hello", "x");
  ck_assert_uint_eq(result, 5);
}
END_TEST

START_TEST(test_strerror_valid) {
  ck_assert_str_eq(strerror(0), s21_strerror(0));
  ck_assert_str_eq(strerror(1), s21_strerror(1));
  ck_assert_str_eq(strerror(2), s21_strerror(2));
  ck_assert_str_eq(strerror(22), s21_strerror(22));
}
END_TEST

START_TEST(test_strerror_invalid) {
  ck_assert_ptr_nonnull(s21_strerror(-1));
  ck_assert_ptr_nonnull(s21_strerror(9999));
  ck_assert_ptr_nonnull(s21_strerror(INT_MAX));
}
END_TEST

START_TEST(test_strlen_basic) {
  ck_assert_uint_eq(strlen(""), s21_strlen(""));
  ck_assert_uint_eq(strlen("X"), s21_strlen("X"));
  ck_assert_uint_eq(strlen("Hello"), s21_strlen("Hello"));
  ck_assert_uint_eq(strlen("Hello World"), s21_strlen("Hello World"));
  ck_assert_uint_eq(strlen("1234567890"), s21_strlen("1234567890"));
  ck_assert_uint_eq(strlen("!@#$%^&*()"), s21_strlen("!@#$%^&*()"));
}
END_TEST

START_TEST(test_strlen_escape) {
  ck_assert_uint_eq(strlen("abc\t\n\r\v\f"), s21_strlen("abc\t\n\r\v\f"));
  ck_assert_uint_eq(strlen("abc\x7F"), s21_strlen("abc\x7F"));
  ck_assert_uint_eq(strlen("Hello\tWorld\n"), s21_strlen("Hello\tWorld\n"));
  ck_assert_uint_eq(strlen("a"), s21_strlen("a"));
  ck_assert_uint_eq(strlen("ab"), s21_strlen("ab"));
  ck_assert_uint_eq(strlen("abcdefghijklmnopqrstuvwxyz"),
                    s21_strlen("abcdefghijklmnopqrstuvwxyz"));
}
END_TEST

START_TEST(test_strpbrk_found) {
  const char* s = "Hello, World!";
  ck_assert_ptr_eq(strpbrk(s, "H"), s21_strpbrk(s, "H"));
  ck_assert_ptr_eq(strpbrk(s, "lo"), s21_strpbrk(s, "lo"));
  ck_assert_ptr_eq(strpbrk(s, "Wrd"), s21_strpbrk(s, "Wrd"));
  ck_assert_ptr_eq(strpbrk(s, " \t\n"), s21_strpbrk(s, " \t\n"));
  ck_assert_ptr_eq(strpbrk(s, "!@#"), s21_strpbrk(s, "!@#"));
}
END_TEST

START_TEST(test_strpbrk_not_found) {
  const char* s = "Hello, World!";
  ck_assert_ptr_eq(strpbrk(s, "xyz"), s21_strpbrk(s, "xyz"));
  ck_assert_ptr_eq(strpbrk(s, "h"), s21_strpbrk(s, "h"));
  ck_assert_ptr_eq(strpbrk("", "abc"), s21_strpbrk("", "abc"));
  ck_assert_ptr_eq(strpbrk("abc", "\x80\x81"), s21_strpbrk("abc", "\x80\x81"));
  ck_assert_ptr_eq(strpbrk("abc", "\x7F"), s21_strpbrk("abc", "\x7F"));
}
END_TEST

START_TEST(test_strpbrk_numbers) {
  const char* s1 = "abc123def456";
  ck_assert_ptr_eq(strpbrk(s1, "0123456789"), s21_strpbrk(s1, "0123456789"));
  ck_assert_ptr_eq(strpbrk("abcdef", "abc"), s21_strpbrk("abcdef", "abc"));
  ck_assert_ptr_eq(strpbrk("abcdef", "cba"), s21_strpbrk("abcdef", "cba"));
  ck_assert_ptr_eq(strpbrk("Hello World", "lo"),
                   s21_strpbrk("Hello World", "lo"));
}
END_TEST

START_TEST(test_strrchr_basic) {
  const char* s = "Hello, World!";
  ck_assert_ptr_eq(strrchr(s, 'H'), s21_strrchr(s, 'H'));
  ck_assert_ptr_eq(strrchr(s, 'o'), s21_strrchr(s, 'o'));
  ck_assert_ptr_eq(strrchr(s, '!'), s21_strrchr(s, '!'));
  ck_assert_ptr_eq(strrchr(s, '\0'), s21_strrchr(s, '\0'));
  ck_assert_ptr_eq(strrchr(s, 'z'), s21_strrchr(s, 'z'));
  ck_assert_ptr_eq(strrchr(s, 'l'), s21_strrchr(s, 'l'));
  ck_assert_ptr_eq(strrchr(s, ' '), s21_strrchr(s, ' '));
}
END_TEST

START_TEST(test_strrchr_edge) {
  const char* s1 = "Hello, World!";
  ck_assert_ptr_eq(strrchr(s1, 'x'), s21_strrchr(s1, 'x'));

  const char* s2 = "!@#$%^&*()";
  ck_assert_ptr_eq(strrchr(s2, '#'), s21_strrchr(s2, '#'));

  const char* s3 = "aba";
  ck_assert_ptr_eq(strrchr(s3, 'a'), s21_strrchr(s3, 'a'));

  const char* s4 = "";
  ck_assert_ptr_eq(strrchr(s4, 'a'), s21_strrchr(s4, 'a'));
  ck_assert_ptr_eq(strrchr(s4, '\0'), s21_strrchr(s4, '\0'));
}
END_TEST

START_TEST(test_strstr_found) {
  const char* hay = "Hello, World!";
  ck_assert_ptr_eq(strstr(hay, "Hello"), s21_strstr(hay, "Hello"));
  ck_assert_ptr_eq(strstr(hay, "World"), s21_strstr(hay, "World"));
  ck_assert_ptr_eq(strstr(hay, "World!"), s21_strstr(hay, "World!"));
  ck_assert_ptr_eq(strstr(hay, "H"), s21_strstr(hay, "H"));
  ck_assert_ptr_eq(strstr(hay, "lo, W"), s21_strstr(hay, "lo, W"));
  ck_assert_ptr_eq(strstr(hay, " "), s21_strstr(hay, " "));
}
END_TEST

START_TEST(test_strstr_repeat) {
  const char* s1 = "abababa";
  ck_assert_ptr_eq(strstr(s1, "aba"), s21_strstr(s1, "aba"));

  const char* s2 = "abcabc";
  ck_assert_ptr_eq(strstr(s2, "abc"), s21_strstr(s2, "abc"));

  const char* s3 = "abacabad";
  ck_assert_ptr_eq(strstr(s3, "aba"), s21_strstr(s3, "aba"));
}
END_TEST

static void check_strtok(char* s1, char* s2, const char* delim) {
  char* tok1[64] = {NULL};
  char* tok2[64] = {NULL};
  int n1 = 0, n2 = 0;

  for (char* t = strtok(s1, delim); t != NULL; t = strtok(NULL, delim))
    tok1[n1++] = t;
  for (char* t = s21_strtok(s2, delim); t != NULL; t = s21_strtok(NULL, delim))
    tok2[n2++] = t;

  ck_assert_int_eq(n1, n2);
  for (int i = 0; i < n1; i++) ck_assert_str_eq(tok1[i], tok2[i]);
}

START_TEST(test_strtok_basic) {
  char s1[] = "Hello,World,How,Are,You";
  char s2[] = "Hello,World,How,Are,You";
  check_strtok(s1, s2, ",");

  char s3[] = "Hello,,World,,,How,Are,You";
  char s4[] = "Hello,,World,,,How,Are,You";
  check_strtok(s3, s4, ",");
}
END_TEST

START_TEST(test_strtok_leading_trailing) {
  char s1[] = ",,,Hello,World";
  char s2[] = ",,,Hello,World";
  check_strtok(s1, s2, ",");

  char s3[] = "Hello,World,,,";
  char s4[] = "Hello,World,,,";
  check_strtok(s3, s4, ",");
}
END_TEST

START_TEST(test_strtok_multi_delim) {
  char s1[] = "Hello World\tHow\nAre You";
  char s2[] = "Hello World\tHow\nAre You";
  check_strtok(s1, s2, " \t\n");

  char s3[] = "Hello!@#World$%How^&Are*You";
  char s4[] = "Hello!@#World$%How^&Are*You";
  check_strtok(s3, s4, "!@#$%^&*");
}
END_TEST

START_TEST(test_strtok_edge) {
  char s1[] = "Hello";
  char s2[] = "Hello";
  check_strtok(s1, s2, ",");

  char s3[] = ",,,";
  char s4[] = ",,,";
  check_strtok(s3, s4, ",");

  char s5[] = "a,b,c,d,e,f";
  char s6[] = "a,b,c,d,e,f";
  check_strtok(s5, s6, ",");
}
END_TEST

START_TEST(test_sprintf_char) {
  char s1[256], s2[256];

  sprintf(s1, "%c", 'A');
  s21_sprintf(s2, "%c", 'A');
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%c", 69);
  s21_sprintf(s2, "%c", 69);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%5c", 'C');
  s21_sprintf(s2, "%5c", 'C');
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-5c", 'X');
  s21_sprintf(s2, "%-5c", 'X');
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_int_basic) {
  char s1[256], s2[256];

  sprintf(s1, "%d", 42);
  s21_sprintf(s2, "%d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%d", 0);
  s21_sprintf(s2, "%d", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%d", -42);
  s21_sprintf(s2, "%d", -42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%d", INT_MAX);
  s21_sprintf(s2, "%d", INT_MAX);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%d", INT_MIN);
  s21_sprintf(s2, "%d", INT_MIN);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_int_flags) {
  char s1[256], s2[256];

  sprintf(s1, "%+d", 42);
  s21_sprintf(s2, "%+d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "% d", 42);
  s21_sprintf(s2, "% d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-5d", 42);
  s21_sprintf(s2, "%-5d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%10.3d", 42);
  s21_sprintf(s2, "%10.3d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-+10.5d", 42);
  s21_sprintf(s2, "%-+10.5d", 42);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_int_length) {
  char s1[256], s2[256];

  sprintf(s1, "%hd", (short)30000);
  s21_sprintf(s2, "%hd", (short)30000);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%ld", 2147483647L);
  s21_sprintf(s2, "%ld", 2147483647L);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_float_flags) {
  char s1[256], s2[256];

  sprintf(s1, "%+f", 123.456789);
  s21_sprintf(s2, "%+f", 123.456789);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "% f", 123.456789);
  s21_sprintf(s2, "% f", 123.456789);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-15f", 123.456789);
  s21_sprintf(s2, "%-15f", 123.456789);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%12.3f", 0.000123456);
  s21_sprintf(s2, "%12.3f", 0.000123456);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_string_basic) {
  char s1[256], s2[256];

  sprintf(s1, "%s", "hello");
  s21_sprintf(s2, "%s", "hello");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%s", "");
  s21_sprintf(s2, "%s", "");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-10s", "abc");
  s21_sprintf(s2, "%-10s", "abc");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%10s", "abc");
  s21_sprintf(s2, "%10s", "abc");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.*s", 5, "longerstring");
  s21_sprintf(s2, "%.*s", 5, "longerstring");
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_unsigned) {
  char s1[256], s2[256];

  sprintf(s1, "%u", 42U);
  s21_sprintf(s2, "%u", 42U);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%u", 0U);
  s21_sprintf(s2, "%u", 0U);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%u", UINT_MAX);
  s21_sprintf(s2, "%u", UINT_MAX);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-5u", 42U);
  s21_sprintf(s2, "%-5u", 42U);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%10.3u", 42U);
  s21_sprintf(s2, "%10.3u", 42U);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_percent) {
  char s1[256], s2[256];

  sprintf(s1, "%%");
  s21_sprintf(s2, "%%");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%s%%", "test");
  s21_sprintf(s2, "%s%%", "test");
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sprintf_all) {
  char s1[1024], s2[1024];

  sprintf(s1, "%d", 5);
  s21_sprintf(s2, "%d", 5);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%i", -10);
  s21_sprintf(s2, "%i", -10);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%u", 100);
  s21_sprintf(s2, "%u", 100);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%x", 255);
  s21_sprintf(s2, "%x", 255);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%X", 255);
  s21_sprintf(s2, "%X", 255);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%o", 64);
  s21_sprintf(s2, "%o", 64);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%c", 'A');
  s21_sprintf(s2, "%c", 'A');
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%s", "Hello, World!");
  s21_sprintf(s2, "%s", "Hello, World!");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%f", 3.14159);
  s21_sprintf(s2, "%f", 3.14159);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%e", 123.456);
  s21_sprintf(s2, "%e", 123.456);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%E", 123.456);
  s21_sprintf(s2, "%E", 123.456);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%g", 123.456);
  s21_sprintf(s2, "%g", 123.456);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%G", 123.456);
  s21_sprintf(s2, "%G", 123.456);
  ck_assert_str_eq(s1, s2);

  int x = 42;
  sprintf(s1, "%p", &x);
  s21_sprintf(s2, "%p", &x);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%%");
  s21_sprintf(s2, "%%");
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%10d", 5);
  s21_sprintf(s2, "%10d", 5);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-10d", 5);
  s21_sprintf(s2, "%-10d", 5);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-*d", -10, 5);
  s21_sprintf(s2, "%-*d", -10, 5);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.5d", 5);
  s21_sprintf(s2, "%.5d", 5);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.3f", 3.14);
  s21_sprintf(s2, "%.3f", 3.14);
  ck_assert_str_eq(s1, s2);

  long l = 123456;
  sprintf(s1, "%ld", l);
  s21_sprintf(s2, "%ld", l);
  ck_assert_str_eq(s1, s2);

  long long int ll = 123456;
  sprintf(s1, "%lld", ll);
  s21_sprintf(s2, "%lld", ll);
  ck_assert_str_eq(s1, s2);

  short s = 123;
  sprintf(s1, "%hd", s);
  s21_sprintf(s2, "%hd", s);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%+10.5d", 42);
  s21_sprintf(s2, "%+10.5d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#x", 255);
  s21_sprintf(s2, "%#x", 255);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#o", 64);
  s21_sprintf(s2, "%#o", 64);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%d %s %c %f", 42, "test", 'X', 3.14);
  s21_sprintf(s2, "%d %s %c %f", 42, "test", 'X', 3.14);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "Простая строка без форматов");
  s21_sprintf(s2, "Простая строка без форматов");
  ck_assert_str_eq(s1, s2);

  int n1, n2;
  sprintf(s1, "Hello %n World", &n1);
  s21_sprintf(s2, "Hello %n World", &n2);
  ck_assert_str_eq(s1, s2);
  ck_assert_int_eq(n1, n2);

  sprintf(s1, "%0*d", 10, 42);
  s21_sprintf(s2, "%0*d", 10, 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "% d", 42);
  s21_sprintf(s2, "% d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%+d", 42);
  s21_sprintf(s2, "%+d", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%ld", 1234567890L);
  s21_sprintf(s2, "%ld", 1234567890L);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%lld", 1234567890123LL);
  s21_sprintf(s2, "%lld", 1234567890123LL);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%e", 0.000123);
  s21_sprintf(s2, "%e", 0.000123);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%g", 123.0);
  s21_sprintf(s2, "%g", 123.0);
  ck_assert_str_eq(s1, s2);

  unsigned char uc = 255;
  sprintf(s1, "%hhu", uc);
  s21_sprintf(s2, "%hhu", uc);
  ck_assert_str_eq(s1, s2);

  unsigned short us = 65535;
  sprintf(s1, "%hu", us);
  s21_sprintf(s2, "%hu", us);
  ck_assert_str_eq(s1, s2);

  unsigned long ul = 4294967295UL;
  sprintf(s1, "%lu", ul);
  s21_sprintf(s2, "%lu", ul);
  ck_assert_str_eq(s1, s2);

  unsigned long long ull = 18446744073709551615ULL;
  sprintf(s1, "%llu", ull);
  s21_sprintf(s2, "%llu", ull);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.0o", 0);
  s21_sprintf(s2, "%.0o", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#.0o", 0);
  s21_sprintf(s2, "%#.0o", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#o", 123);
  s21_sprintf(s2, "%#o", 123);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#o", 01234567);
  s21_sprintf(s2, "%#o", 01234567);
  ck_assert_str_eq(s1, s2);

  unsigned char oc = 077;
  sprintf(s1, "%#ho", oc);
  s21_sprintf(s2, "%#ho", oc);
  ck_assert_str_eq(s1, s2);

  unsigned long ol = 0123456701234567UL;
  sprintf(s1, "%#lo", ol);
  s21_sprintf(s2, "%#lo", ol);
  ck_assert_str_eq(s1, s2);

  unsigned char hc = 0xFF;
  sprintf(s1, "%hhx", hc);
  s21_sprintf(s2, "%hhx", hc);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%hhX", hc);
  s21_sprintf(s2, "%hhX", hc);
  ck_assert_str_eq(s1, s2);

  unsigned short hs = 0xFFFF;
  sprintf(s1, "%hx", hs);
  s21_sprintf(s2, "%hx", hs);
  ck_assert_str_eq(s1, s2);

  unsigned long hl = 0xFFFFFFFFUL;
  sprintf(s1, "%lx", hl);
  s21_sprintf(s2, "%lx", hl);
  ck_assert_str_eq(s1, s2);

  unsigned long long hll = 0xFFFFFFFFFFFFFFFFULL;
  sprintf(s1, "%llx", hll);
  s21_sprintf(s2, "%llx", hll);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#x", 0xFF);
  s21_sprintf(s2, "%#x", 0xFF);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#X", 0xFF);
  s21_sprintf(s2, "%#X", 0xFF);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.0u", 0);
  s21_sprintf(s2, "%.0u", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.0x", 0);
  s21_sprintf(s2, "%.0x", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.0X", 0);
  s21_sprintf(s2, "%.0X", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.0u", 42);
  s21_sprintf(s2, "%.0u", 42);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%5.0u", 0);
  s21_sprintf(s2, "%5.0u", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.10x", 0xFF);
  s21_sprintf(s2, "%.10x", 0xFF);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#.10x", 0xFF);
  s21_sprintf(s2, "%#.10x", 0xFF);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#.10X", 0xFF);
  s21_sprintf(s2, "%#.10X", 0xFF);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.20x", 0xABC);
  s21_sprintf(s2, "%.20x", 0xABC);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#x", 0);
  s21_sprintf(s2, "%#x", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%.1x", 0x35);
  s21_sprintf(s2, "%.1x", 0x35);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#X", 0);
  s21_sprintf(s2, "%#X", 0);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#x", 0xABC);
  s21_sprintf(s2, "%#x", 0xABC);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#.5o", 0123);
  s21_sprintf(s2, "%#.5o", 0123);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#10.5o", 0123);
  s21_sprintf(s2, "%#10.5o", 0123);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%-#10o", 0123);
  s21_sprintf(s2, "%-#10o", 0123);
  ck_assert_str_eq(s1, s2);

  sprintf(s1, "%#o", 0);
  s21_sprintf(s2, "%#o", 0);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_trim_spaces) {
  char* r;

  r = s21_trim("", "");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "");
  free(r);

  r = s21_trim("   Hello, World!", " ");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("Hello, World!   ", " ");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("   Hello, World!   ", " ");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);
}
END_TEST

START_TEST(test_trim_custom_chars) {
  char* r;

  r = s21_trim("***Hello, World!***", "*");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("[[[Hello, World!]]]", "[]");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("123Hello, World!123", "123");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("Hello, World!", "*");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);
}
END_TEST

START_TEST(test_trim_whitespace) {
  char* r;

  r = s21_trim("\t\tHello, World!", "\t");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("\t\n Hello, World! \n\t", " \t\n");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello, World!");
  free(r);

  r = s21_trim("Hello", "");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello");
  free(r);
}
END_TEST

START_TEST(test_trim_edge) {
  char* r;

  r = s21_trim("Hello, World!", "Hello, World! ");
  ck_assert_ptr_nonnull(r);
  free(r);

  r = s21_trim("   ", " ");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "");
  free(r);

  r = s21_trim("**********", "*");
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "");
  free(r);
}
END_TEST

static void check_toupper(const char* input) {
  if (input == NULL) {
    char* result = s21_toupper(input);
    ck_assert_ptr_null(result);
    return;
  }

  char* result = s21_toupper(input);
  ck_assert_ptr_nonnull(result);
  size_t len = strlen(input);
  for (size_t i = 0; i < len; i++) {
    ck_assert_int_eq((unsigned char)result[i],
                     toupper((unsigned char)input[i]));
  }
  free(result);
}

START_TEST(test_toupper_basic) {
  check_toupper("");
  check_toupper("hello world");
  check_toupper("HELLO WORLD");
  check_toupper("Hello World");
  check_toupper("1234567890");
  check_toupper("!@#$%^&*()");
  check_toupper("Hello World 123");
  check_toupper("abcdefghijklmnopqrstuvwxyz");
  check_toupper("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  check_toupper(NULL);
}
END_TEST

START_TEST(test_toupper_mixed) {
  check_toupper("a");
  check_toupper("A");
  check_toupper("AbCdEfGhIjK");
  check_toupper("abc\x7F");
  check_toupper("Hello\tWorld\n");
  check_toupper("abc123def456");
}
END_TEST

static void check_tolower(const char* input) {
  if (input == NULL) {
    char* result = s21_tolower(input);
    ck_assert_ptr_null(result);
    return;
  }

  char* result = s21_tolower(input);
  ck_assert_ptr_nonnull(result);
  size_t len = strlen(input);
  for (size_t i = 0; i < len; i++) {
    ck_assert_int_eq((unsigned char)result[i],
                     tolower((unsigned char)input[i]));
  }
  free(result);
}

START_TEST(test_tolower_basic) {
  check_tolower("");
  check_tolower("HELLO WORLD");
  check_tolower("hello world");
  check_tolower("Hello World");
  check_tolower("1234567890");
  check_tolower("!@#$%^&*()");
  check_tolower("Hello World 123");
  check_tolower("abcdefghijklmnopqrstuvwxyz");
  check_tolower("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}
END_TEST

START_TEST(test_tolower_mixed) {
  check_tolower("A");
  check_tolower("a");
  check_tolower("AbCdEfGhIjK");
  check_tolower("abc\x7F");
  check_tolower("Hello\tWorld\n");
  check_tolower("ABC123DEF456");
  check_tolower(NULL);
}
END_TEST

START_TEST(test_insert_basic) {
  char* r;

  r = s21_insert("Hello", "World", 5);
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "HelloWorld");
  free(r);

  r = s21_insert("Hello", "World", 0);
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "WorldHello");
  free(r);

  r = s21_insert("Hello", "World", 3);
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "HelWorldlo");
  free(r);

  r = s21_insert("Hello", "", 2);
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello");
  free(r);
}
END_TEST

START_TEST(test_insert_empty) {
  char* r;

  r = s21_insert("", "Hello", 0);
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "Hello");
  free(r);

  r = s21_insert("", "", 0);
  ck_assert_ptr_nonnull(r);
  ck_assert_str_eq(r, "");
  free(r);
}
END_TEST

START_TEST(test_insert_invalid) {
  ck_assert_ptr_null(s21_insert("Hello", "World", 10));
  ck_assert_ptr_null(s21_insert(NULL, "World", 0));
}
END_TEST

START_TEST(test_sscanf_int_basic) {
  int a1, a2;
  int r1, r2;

  r1 = sscanf("42", "%d", &a1);
  r2 = s21_sscanf("42", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("-42", "%d", &a1);
  r2 = s21_sscanf("-42", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("+100", "%d", &a1);
  r2 = s21_sscanf("+100", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("0", "%d", &a1);
  r2 = s21_sscanf("0", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("  99", "%d", &a1);
  r2 = s21_sscanf("  99", "%d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

START_TEST(test_sscanf_int_width) {
  int a1, a2;

  int r1 = sscanf("12345", "%3d", &a1);
  int r2 = s21_sscanf("12345", "%3d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("-12345", "%4d", &a1);
  r2 = s21_sscanf("-12345", "%4d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

START_TEST(test_sscanf_int_length) {
  short s1, s2;
  long l1, l2;
  long long ll1, ll2;

  int r1 = sscanf("32767", "%hd", &s1);
  int r2 = s21_sscanf("32767", "%hd", &s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(s1, s2);

  r1 = sscanf("2147483647", "%ld", &l1);
  r2 = s21_sscanf("2147483647", "%ld", &l2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq((long long)l1, (long long)l2);

  r1 = sscanf("9223372036854775807", "%lld", &ll1);
  r2 = s21_sscanf("9223372036854775807", "%lld", &ll2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(ll1, ll2);
}
END_TEST

START_TEST(test_sscanf_i_auto_base) {
  int a1, a2;

  int r1 = sscanf("0x1A", "%i", &a1);
  int r2 = s21_sscanf("0x1A", "%i", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("010", "%i", &a1);
  r2 = s21_sscanf("010", "%i", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  r1 = sscanf("255", "%i", &a1);
  r2 = s21_sscanf("255", "%i", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
}
END_TEST

START_TEST(test_sscanf_unsigned) {
  unsigned u1, u2;

  int r1 = sscanf("42", "%u", &u1);
  int r2 = s21_sscanf("42", "%u", &u2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(u1, u2);

  r1 = sscanf("4294967295", "%u", &u1);
  r2 = s21_sscanf("4294967295", "%u", &u2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(u1, u2);

  r1 = sscanf("0", "%u", &u1);
  r2 = s21_sscanf("0", "%u", &u2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(u1, u2);
}
END_TEST

START_TEST(test_sscanf_octal) {
  unsigned o1, o2;

  int r1 = sscanf("17", "%o", &o1);
  int r2 = s21_sscanf("17", "%o", &o2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(o1, o2);

  r1 = sscanf("0", "%o", &o1);
  r2 = s21_sscanf("0", "%o", &o2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(o1, o2);

  r1 = sscanf("777", "%o", &o1);
  r2 = s21_sscanf("777", "%o", &o2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(o1, o2);
}
END_TEST

START_TEST(test_sscanf_hex) {
  unsigned x1, x2;

  int r1 = sscanf("ff", "%x", &x1);
  int r2 = s21_sscanf("ff", "%x", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(x1, x2);

  r1 = sscanf("0xFF", "%x", &x1);
  r2 = s21_sscanf("0xFF", "%x", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(x1, x2);

  r1 = sscanf("1A2B", "%X", &x1);
  r2 = s21_sscanf("1A2B", "%X", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(x1, x2);

  r1 = sscanf("0", "%x", &x1);
  r2 = s21_sscanf("0", "%x", &x2);
  ck_assert_int_eq(r1, r2);
  ck_assert_uint_eq(x1, x2);
}
END_TEST

START_TEST(test_sscanf_float) {
  float f1, f2;

  int r1 = sscanf("3.14", "%f", &f1);
  int r2 = s21_sscanf("3.14", "%f", &f2);
  ck_assert_int_eq(r1, r2);
  ck_assert_float_eq_tol(f1, f2, 1e-5f);

  r1 = sscanf("-2.718", "%f", &f1);
  r2 = s21_sscanf("-2.718", "%f", &f2);
  ck_assert_int_eq(r1, r2);
  ck_assert_float_eq_tol(f1, f2, 1e-5f);

  r1 = sscanf("0.0", "%f", &f1);
  r2 = s21_sscanf("0.0", "%f", &f2);
  ck_assert_int_eq(r1, r2);
  ck_assert_float_eq_tol(f1, f2, 1e-5f);

  r1 = sscanf("1e3", "%f", &f1);
  r2 = s21_sscanf("1e3", "%f", &f2);
  ck_assert_int_eq(r1, r2);
  ck_assert_float_eq_tol(f1, f2, 1.0f);
}
END_TEST

START_TEST(test_sscanf_float_long_double) {
  double d1, d2;

  int r1 = sscanf("3.141592653589793", "%lf", &d1);
  int r2 = s21_sscanf("3.141592653589793", "%lf", &d2);
  ck_assert_int_eq(r1, r2);
  ck_assert_double_eq_tol(d1, d2, 1e-9);
}
END_TEST

START_TEST(test_sscanf_char) {
  char c1, c2;

  int r1 = sscanf("A", "%c", &c1);
  int r2 = s21_sscanf("A", "%c", &c2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(c1, c2);

  r1 = sscanf(" X", "%c", &c1);
  r2 = s21_sscanf(" X", "%c", &c2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(c1, c2);

  r1 = sscanf("!", "%c", &c1);
  r2 = s21_sscanf("!", "%c", &c2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(c1, c2);
}
END_TEST

START_TEST(test_sscanf_string) {
  char s1[64], s2[64];

  int r1 = sscanf("hello", "%s", s1);
  int r2 = s21_sscanf("hello", "%s", s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);

  r1 = sscanf("  world", "%s", s1);
  r2 = s21_sscanf("  world", "%s", s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);

  r1 = sscanf("abc def", "%s", s1);
  r2 = s21_sscanf("abc def", "%s", s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sscanf_string_width) {
  char s1[64], s2[64];

  int r1 = sscanf("hello", "%3s", s1);
  int r2 = s21_sscanf("hello", "%3s", s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sscanf_multiple) {
  int a1, a2;
  float f1, f2;
  char s1[64], s2[64];

  int r1 = sscanf("42 3.14 hello", "%d %f %s", &a1, &f1, s1);
  int r2 = s21_sscanf("42 3.14 hello", "%d %f %s", &a2, &f2, s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_float_eq_tol(f1, f2, 1e-5f);
  ck_assert_str_eq(s1, s2);

  int b1, b2, c1, c2;
  r1 = sscanf("10 20 30", "%d %d %d", &a1, &b1, &c1);
  r2 = s21_sscanf("10 20 30", "%d %d %d", &a2, &b2, &c2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_int_eq(b1, b2);
  ck_assert_int_eq(c1, c2);
}
END_TEST

START_TEST(test_sscanf_suppress) {
  int a1, a2;

  int r1 = sscanf("10 20", "%*d %d", &a1);
  int r2 = s21_sscanf("10 20", "%*d %d", &a2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);

  char s1[64], s2[64];
  r1 = sscanf("hello world", "%*s %s", s1);
  r2 = s21_sscanf("hello world", "%*s %s", s2);
  ck_assert_int_eq(r1, r2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sscanf_n) {
  int a1, a2, n1 = 0, n2 = 0;

  int r1 = sscanf("42 hello", "%d%n", &a1, &n1);
  int r2 = s21_sscanf("42 hello", "%d%n", &a2, &n2);
  ck_assert_int_eq(r1, r2);
  ck_assert_int_eq(a1, a2);
  ck_assert_int_eq(n1, n2);
}
END_TEST

START_TEST(test_sscanf_d_basic) {
  int d1, d2;
  int n1, n2;

  n1 = sscanf("123", "%d", &d1);
  n2 = s21_sscanf("123", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);

  n1 = sscanf("-456", "%d", &d1);
  n2 = s21_sscanf("-456", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);

  n1 = sscanf("+789", "%d", &d1);
  n2 = s21_sscanf("+789", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);

  n1 = sscanf("0", "%d", &d1);
  n2 = s21_sscanf("0", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);
}
END_TEST

START_TEST(test_sscanf_d_whitespace) {
  int d1, d2;
  int n1, n2;

  n1 = sscanf("  \t\n123", "%d", &d1);
  n2 = s21_sscanf("  \t\n123", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);

  n1 = sscanf("\t  -456", "%d", &d1);
  n2 = s21_sscanf("\t  -456", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);
}
END_TEST

START_TEST(test_sscanf_d_width) {
  int d1, d2;
  int n1, n2;

  n1 = sscanf("12345", "%3d", &d1);
  n2 = s21_sscanf("12345", "%3d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);

  n1 = sscanf("-12345", "%4d", &d1);
  n2 = s21_sscanf("-12345", "%4d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);

  n1 = sscanf("+12345", "%4d", &d1);
  n2 = s21_sscanf("+12345", "%4d", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);
}
END_TEST

START_TEST(test_sscanf_d_length) {
  signed char sc1, sc2;
  short sh1, sh2;
  long l1, l2;
  long long ll1, ll2;
  int n1, n2;

  n1 = sscanf("127", "%hhd", &sc1);
  n2 = s21_sscanf("127", "%hhd", &sc2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(sc1, sc2);

  n1 = sscanf("32767", "%hd", &sh1);
  n2 = s21_sscanf("32767", "%hd", &sh2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(sh1, sh2);

  n1 = sscanf("2147483647", "%ld", &l1);
  n2 = s21_sscanf("2147483647", "%ld", &l2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(l1, l2);

  n1 = sscanf("9223372036854775807", "%lld", &ll1);
  n2 = s21_sscanf("9223372036854775807", "%lld", &ll2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(ll1, ll2);
}
END_TEST

START_TEST(test_sscanf_d_overflow) {
  int d1, d2;
  int n1, n2;

  n1 = sscanf("999999999999", "%d", &d1);
  n2 = s21_sscanf("999999999999", "%d", &d2);
  ck_assert_int_eq(n1, n2);
  (void)d1;
  (void)d2;
}
END_TEST

START_TEST(test_sscanf_i_width) {
  int i1, i2;
  int n1, n2;

  n1 = sscanf("0x1A2B", "%4i", &i1);
  n2 = s21_sscanf("0x1A2B", "%4i", &i2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(i1, i2);

  n1 = sscanf("012345", "%3i", &i1);
  n2 = s21_sscanf("012345", "%3i", &i2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(i1, i2);
}
END_TEST

START_TEST(test_sscanf_u_basic) {
  unsigned u1, u2;
  int n1, n2;

  n1 = sscanf("123", "%u", &u1);
  n2 = s21_sscanf("123", "%u", &u2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(u1, u2);

  n1 = sscanf("4294967295", "%u", &u1);
  n2 = s21_sscanf("4294967295", "%u", &u2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(u1, u2);

  n1 = sscanf("0", "%u", &u1);
  n2 = s21_sscanf("0", "%u", &u2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(u1, u2);
}
END_TEST

START_TEST(test_sscanf_u_length) {
  unsigned char uc1 = 0, uc2 = 0;
  unsigned short us1, us2;
  unsigned long ul1, ul2;
  unsigned long long ull1, ull2;
  int n1, n2;

  // Тест 1: Простой случай с %% в начале строки
  n1 = sscanf("%%", "%%");
  n2 = s21_sscanf("%%", "%%");
  ck_assert_int_eq(n1, n2);

  n1 = sscanf("255", "%hhu", &uc1);
  n2 = s21_sscanf("255", "%hhu", &uc2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(uc1, uc2);

  n1 = sscanf("65535", "%hu", &us1);
  n2 = s21_sscanf("65535", "%hu", &us2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(us1, us2);

  n1 = sscanf("4294967295", "%lu", &ul1);
  n2 = s21_sscanf("4294967295", "%lu", &ul2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(ul1, ul2);

  n1 = sscanf("18446744073709551615", "%llu", &ull1);
  n2 = s21_sscanf("18446744073709551615", "%llu", &ull2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(ull1, ull2);
}
END_TEST

START_TEST(test_sscanf_o_basic) {
  unsigned o1, o2;
  int n1, n2;

  n1 = sscanf("17", "%o", &o1);
  n2 = s21_sscanf("17", "%o", &o2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(o1, o2);

  n1 = sscanf("0", "%o", &o1);
  n2 = s21_sscanf("0", "%o", &o2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(o1, o2);

  n1 = sscanf("777", "%o", &o1);
  n2 = s21_sscanf("777", "%o", &o2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(o1, o2);

  n1 = sscanf("012", "%o", &o1);
  n2 = s21_sscanf("012", "%o", &o2);
  ck_assert_int_eq(n1, n2);
  ck_assert_uint_eq(o1, o2);
}
END_TEST

START_TEST(test_sscanf_p_basic) {
  void* p1;
  void* p2;
  int n1, n2;

  n1 = sscanf("0x7ffee2d6b0a0", "%p", &p1);
  n2 = s21_sscanf("0x7ffee2d6b0a0", "%p", &p2);
  ck_assert_int_eq(n1, n2);
  ck_assert_ptr_eq(p1, p2);
  n1 = sscanf("0x0", "%p", &p1);
  n2 = s21_sscanf("0x0", "%p", &p2);
  ck_assert_int_eq(n1, n2);
  ck_assert_ptr_eq(p1, p2);

  n1 = sscanf("0xFFFF", "%p", &p1);
  n2 = s21_sscanf("0xFFFF", "%p", &p2);
  ck_assert_int_eq(n1, n2);
  ck_assert_ptr_eq(p1, p2);
}
END_TEST

START_TEST(test_sscanf_f_basic) {
  float f1, f2;
  int n1, n2;

  n1 = sscanf("3.14", "%f", &f1);
  n2 = s21_sscanf("3.14", "%f", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);

  n1 = sscanf("-2.718", "%f", &f1);
  n2 = s21_sscanf("-2.718", "%f", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);

  n1 = sscanf("+1.0", "%f", &f1);
  n2 = s21_sscanf("+1.0", "%f", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);

  n1 = sscanf("0.0", "%f", &f1);
  n2 = s21_sscanf("0.0", "%f", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);
}
END_TEST

START_TEST(test_sscanf_f_scientific) {
  float f1, f2;
  int n1, n2;

  n1 = sscanf("1e3", "%e", &f1);
  n2 = s21_sscanf("1e3", "%e", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);

  n1 = sscanf("-2.5e-4", "%E", &f1);
  n2 = s21_sscanf("-2.5e-4", "%E", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);

  n1 = sscanf("6.02e23", "%g", &f1);
  n2 = s21_sscanf("6.02e23", "%g", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);
}
END_TEST

START_TEST(test_sscanf_f_double) {
  double d1, d2;
  int n1, n2;

  n1 = sscanf("3.141592653589793", "%lf", &d1);
  n2 = s21_sscanf("3.141592653589793", "%lf", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_double_eq_tol(d1, d2, 1e-9);

  n1 = sscanf("1e-10", "%lf", &d1);
  n2 = s21_sscanf("1e-10", "%lf", &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_double_eq_tol(d1, d2, 1e-15);
}
END_TEST

START_TEST(test_sscanf_f_long_double) {
  long double ld1, ld2;
  int n1, n2;

  n1 = sscanf("3.14159265358979323846", "%Lf", &ld1);
  n2 = s21_sscanf("3.14159265358979323846", "%Lf", &ld2);
  ck_assert_int_eq(n1, n2);
  ck_assert_ldouble_eq_tol(ld1, ld2, 1e-12);
}
END_TEST

START_TEST(test_sscanf_f_width) {
  float f1, f2;
  int n1, n2;

  n1 = sscanf("123.456", "%5f", &f1);
  n2 = s21_sscanf("123.456", "%5f", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);
}
END_TEST

START_TEST(test_sscanf_f_invalid) {
  float f1 = 0, f2 = 0;
  int n1, n2;

  n1 = sscanf("abc", "%f", &f1);
  n2 = s21_sscanf("abc", "%f", &f2);
  ck_assert_int_eq(n1, n2);
  ck_assert_float_eq_tol(f1, f2, 1e-5);
}
END_TEST

START_TEST(test_sscanf_c_width) {
  char str1[10], str2[10];
  int n1, n2;

  n1 = sscanf("Hello", "%3c", str1);
  n2 = s21_sscanf("Hello", "%3c", str2);
  ck_assert_int_eq(n1, n2);
  ck_assert_mem_eq(str1, str2, 3);
}
END_TEST

START_TEST(test_sscanf_s_basic) {
  char s1[64], s2[64];
  int n1, n2;
  n1 = sscanf("hello", "%s", s1);
  n2 = s21_sscanf("hello", "%s", s2);
  ck_assert_int_eq(n1, n2);
  ck_assert_str_eq(s1, s2);

  n1 = sscanf("  world  ", "%s", s1);
  n2 = s21_sscanf("  world  ", "%s", s2);
  ck_assert_int_eq(n1, n2);
  ck_assert_str_eq(s1, s2);

  n1 = sscanf("hello world", "%s", s1);
  n2 = s21_sscanf("hello world", "%s", s2);
  ck_assert_int_eq(n1, n2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sscanf_s_width) {
  char s1[64], s2[64];
  int n1, n2;

  n1 = sscanf("hello", "%3s", s1);
  n2 = s21_sscanf("hello", "%3s", s2);
  ck_assert_int_eq(n1, n2);
  ck_assert_str_eq(s1, s2);

  s1[0] = s2[0] = 0;
  n1 = sscanf("hello", "%10s", s1);
  n2 = s21_sscanf("hello", "%10s", s2);
  ck_assert_int_eq(n1, n2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sscanf_suppress_char) {
  char c1, c2;
  int n1, n2;

  n1 = sscanf("ABC", "%*c %c", &c1);
  n2 = s21_sscanf("ABC", "%*c %c", &c2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(c1, c2);
}
END_TEST

START_TEST(test_sscanf_n_basic) {
  int n_val1 = 0, n_val2 = 0;
  int d1, d2;
  char s1[64], s2[64];
  int n1, n2;

  n1 = sscanf("123 456", "%d %n %d", &d1, &n_val1, &d2);
  n2 = s21_sscanf("123 456", "%d %n %d", &d1, &n_val2, &d2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(n_val1, n_val2);

  n_val1 = n_val2 = 0;
  n1 = sscanf("hello", "%n %s", &n_val1, s1);
  n2 = s21_sscanf("hello", "%n %s", &n_val2, s2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(n_val1, n_val2);
  ck_assert_str_eq(s1, s2);
}
END_TEST

START_TEST(test_sscanf_mixed_bases) {
  int d1, d2;
  unsigned o1, o2;
  unsigned x1, x2;
  int n1, n2;

  n1 = sscanf("123 0xFF 012", "%d %x %o", &d1, &x1, &o1);
  n2 = s21_sscanf("123 0xFF 012", "%d %x %o", &d2, &x2, &o2);
  ck_assert_int_eq(n1, n2);
  ck_assert_int_eq(d1, d2);
  ck_assert_uint_eq(x1, x2);
  ck_assert_uint_eq(o1, o2);
}
END_TEST

START_TEST(test_sscanf_mixed_whitespace) {
  int a1, a2, b1, b2, c1, c2;
  int n1, n2;

  n1 = sscanf("10\t20\n30", "%d %d %d", &a1, &b1, &c1);
  n2 = s21_sscanf("10\t20\n30", "%d %d %d", &a2, &b2, &c2);
  ck_assert_int_eq(n1, n2);
}
END_TEST

static Suite* suite_memchr(void) {
  Suite* s = suite_create("memchr");
  TCase* tc = tcase_create("Core");

  tcase_add_test(tc, test_memchr_found);
  tcase_add_test(tc, test_memchr_not_found);
  tcase_add_test(tc, test_memchr_limit);

  tcase_add_test(tc, test_memchr_null_char);
  tcase_add_test(tc, test_memchr_binary_data);
  tcase_add_test(tc, test_memchr_large_n);
  tcase_add_test(tc, test_memchr_overlap);
  tcase_add_test(tc, test_memchr_performance);

  suite_add_tcase(s, tc);
  return s;
}

Suite* suite_memcmp(void) {
  Suite* s = suite_create("memcmp");
  TCase* tc = tcase_create("Core");
  tcase_add_test(tc, test_memcmp_equal);
  tcase_add_test(tc, test_memcmp_sign);
  tcase_add_test(tc, test_memcmp_embedded_null);

  tcase_add_test(tc, test_memcmp_negative_values);
  tcase_add_test(tc, test_memcmp_max_values);
  tcase_add_test(tc, test_memcmp_different_types);
  tcase_add_test(tc, test_memcmp_large_n);
  tcase_add_test(tc, test_memcmp_early_exit);
  tcase_add_test(tc, test_memcmp_volatile);
  tcase_add_test(tc, test_memcmp_repeating_patterns);
  tcase_add_test(tc, test_memcmp_null_ptrs);

  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_memcpy(void) {
  Suite* s = suite_create("memcpy");

  TCase* tc_basic = tcase_create("Basic");
  tcase_add_test(tc_basic, test_memcpy_basic);
  tcase_add_test(tc_basic, test_memcpy_bytes);
  suite_add_tcase(s, tc_basic);

  TCase* tc_special = tcase_create("Special Cases");
  tcase_add_test(tc_special, test_memcpy_overlap);
  tcase_add_test(tc_special, test_memcpy_boundary);
  suite_add_tcase(s, tc_special);

  TCase* tc_large = tcase_create("Large Data");
  tcase_add_test(tc_large, test_memcpy_large);
  suite_add_tcase(s, tc_large);

  TCase* tc_alignment = tcase_create("Alignment");
  tcase_add_test(tc_alignment, test_memcpy_alignment);
  suite_add_tcase(s, tc_alignment);

  TCase* tc_types = tcase_create("Data Types");
  tcase_add_test(tc_types, test_memcpy_different_types);
  suite_add_tcase(s, tc_types);

  TCase* tc_null = tcase_create("NULL Pointers");
  tcase_add_test(tc_null, test_memcpy_null_ptrs);
  suite_add_tcase(s, tc_null);

  return s;
}

static Suite* suite_memset(void) {
  Suite* s = suite_create("memset");

  TCase* tc_basic = tcase_create("Basic");
  tcase_add_test(tc_basic, test_memset_basic);
  tcase_add_test(tc_basic, test_memset_values);
  suite_add_tcase(s, tc_basic);

  TCase* tc_edge = tcase_create("Edge Cases");
  tcase_add_test(tc_edge, test_memset_edge_values);
  tcase_add_test(tc_edge, test_memset_partial);
  tcase_add_test(tc_edge, test_memset_null_ptr);
  suite_add_tcase(s, tc_edge);

  TCase* tc_large = tcase_create("Large Data");
  tcase_add_test(tc_large, test_memset_large);
  tcase_add_test(tc_large, test_memset_max_size);
  suite_add_tcase(s, tc_large);

  TCase* tc_alignment = tcase_create("Alignment");
  tcase_add_test(tc_alignment, test_memset_alignment);
  suite_add_tcase(s, tc_alignment);

  TCase* tc_multiple = tcase_create("Multiple Calls");
  tcase_add_test(tc_multiple, test_memset_multiple_calls);
  suite_add_tcase(s, tc_multiple);

  return s;
}

static Suite* suite_strncat(void) {
  Suite* s = suite_create("strncat");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strncat_basic);
  tcase_add_test(tc, test_strncat_trunc);
  suite_add_tcase(s, tc);

  TCase* tc_edge = tcase_create("Edge Cases");
  tcase_add_test(tc_edge, test_strncat_null_terminator);
  tcase_add_test(tc_edge, test_strncat_empty_dest);
  tcase_add_test(tc_edge, test_strncat_n_larger_than_src);
  tcase_add_test(tc_edge, test_strncat_exact_fit);
  tcase_add_test(tc_edge, test_strncat_partial_copy);
  tcase_add_test(tc_edge, test_strncat_boundary_values);
  suite_add_tcase(s, tc_edge);

  TCase* tc_special = tcase_create("Special Cases");
  tcase_add_test(tc_special, test_strncat_special_chars);
  tcase_add_test(tc_special, test_strncat_overlap);
  suite_add_tcase(s, tc_special);

  TCase* tc_large = tcase_create("Large Data");
  tcase_add_test(tc_large, test_strncat_large_n);
  suite_add_tcase(s, tc_large);
  return s;
}

static Suite* suite_strchr(void) {
  Suite* s = suite_create("strchr");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strchr_basic);
  tcase_add_test(tc, test_strchr_edge);
  tcase_add_test(tc, test_strchr_first_occurrence);
  suite_add_tcase(s, tc);

  TCase* tc_edge = tcase_create("Edge Cases");
  tcase_add_test(tc_edge, test_strchr_null_char_only);
  tcase_add_test(tc_edge, test_strchr_not_found);
  tcase_add_test(tc_edge, test_strchr_empty_string);
  tcase_add_test(tc_edge, test_strchr_single_char_string);
  tcase_add_test(tc_edge, test_strchr_all_chars_same);
  tcase_add_test(tc_edge, test_strchr_multiple_occurrences);
  suite_add_tcase(s, tc_edge);

  TCase* tc_values = tcase_create("Values");
  tcase_add_test(tc_values, test_strchr_negative_values);
  tcase_add_test(tc_values, test_strchr_max_values);
  tcase_add_test(tc_values, test_strchr_special_chars);
  tcase_add_test(tc_values, test_strchr_unicode);
  suite_add_tcase(s, tc_values);

  TCase* tc_large = tcase_create("Large Data");
  tcase_add_test(tc_large, test_strchr_long_string);
  suite_add_tcase(s, tc_large);

  TCase* tc_ptr = tcase_create("Pointers");
  tcase_add_test(tc_ptr, test_strchr_null_ptr);
  suite_add_tcase(s, tc_ptr);

  return s;
}

static Suite* suite_strncmp(void) {
  Suite* s = suite_create("strncmp");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strncmp_equal);
  tcase_add_test(tc, test_strncmp_sign);
  tcase_add_test(tc, test_strncmp_prefix);
  suite_add_tcase(s, tc);

  TCase* tc_diff = tcase_create("Differences");
  tcase_add_test(tc_diff, test_strncmp_different_at_start);
  tcase_add_test(tc_diff, test_strncmp_different_at_end);
  tcase_add_test(tc_diff, test_strncmp_different_at_end_with_n_limit);
  suite_add_tcase(s, tc_diff);

  TCase* tc_edge = tcase_create("Edge Cases");
  tcase_add_test(tc_edge, test_strncmp_with_null_chars);
  tcase_add_test(tc_edge, test_strncmp_n_zero);
  tcase_add_test(tc_edge, test_strncmp_n_larger_than_strings);
  tcase_add_test(tc_edge, test_strncmp_null_ptrs);
  suite_add_tcase(s, tc_edge);

  TCase* tc_values = tcase_create("Values");
  tcase_add_test(tc_values, test_strncmp_case_sensitive);
  tcase_add_test(tc_values, test_strncmp_unsigned_char_values);
  suite_add_tcase(s, tc_values);

  return s;
}

static Suite* suite_strncpy(void) {
  Suite* s = suite_create("strncpy");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strncpy_truncate);
  suite_add_tcase(s, tc);

  TCase* tc_size = tcase_create("Sizes");
  tcase_add_test(tc_size, test_strncpy_exact_length);
  suite_add_tcase(s, tc_size);

  TCase* tc_special = tcase_create("Special Cases");
  tcase_add_test(tc_special, test_strncpy_null_in_src);
  tcase_add_test(tc_special, test_strncpy_src_empty);
  tcase_add_test(tc_special, test_strncpy_special_chars);
  tcase_add_test(tc_special, test_strncpy_dest_larger_than_src);
  suite_add_tcase(s, tc_special);

  TCase* tc_ptr = tcase_create("Pointers");
  tcase_add_test(tc_ptr, test_strncpy_return_value);
  suite_add_tcase(s, tc_ptr);

  TCase* tc_perf = tcase_create("Performance");
  tcase_add_test(tc_perf, test_strncpy_large_n);
  suite_add_tcase(s, tc_perf);
  return s;
}

static Suite* suite_strcspn(void) {
  Suite* s = suite_create("strcspn");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strcspn_basic);
  tcase_add_test(tc, test_strcspn_chars);
  tcase_add_test(tc, test_strcspn_edge);
  suite_add_tcase(s, tc);
  TCase* tc_found = tcase_create("Found/Not Found");
  tcase_add_test(tc_found, test_strcspn_not_found);
  tcase_add_test(tc_found, test_strcspn_found_at_start);
  tcase_add_test(tc_found, test_strcspn_found_at_end);
  tcase_add_test(tc_found, test_strcspn_found_in_middle);
  tcase_add_test(tc_found, test_strcspn_multiple_chars_in_str2);
  suite_add_tcase(s, tc_found);

  TCase* tc_empty = tcase_create("Empty Strings");
  tcase_add_test(tc_empty, test_strcspn_empty_str1);
  tcase_add_test(tc_empty, test_strcspn_empty_str2);
  tcase_add_test(tc_empty, test_strcspn_both_empty);
  suite_add_tcase(s, tc_empty);

  TCase* tc_special = tcase_create("Special Cases");
  tcase_add_test(tc_special, test_strcspn_special_chars);
  tcase_add_test(tc_special, test_strcspn_with_null_chars);
  suite_add_tcase(s, tc_special);

  TCase* tc_type = tcase_create("Type and Performance");
  tcase_add_test(tc_type, test_strcspn_return_type);
  tcase_add_test(tc_type, test_strcspn_long_strings);
  suite_add_tcase(s, tc_type);
  return s;
}

static Suite* suite_strerror(void) {
  Suite* s = suite_create("strerror");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strerror_valid);
  tcase_add_test(tc, test_strerror_invalid);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_strlen(void) {
  Suite* s = suite_create("strlen");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strlen_basic);
  tcase_add_test(tc, test_strlen_escape);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_strpbrk(void) {
  Suite* s = suite_create("strpbrk");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strpbrk_found);
  tcase_add_test(tc, test_strpbrk_not_found);
  tcase_add_test(tc, test_strpbrk_numbers);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_strrchr(void) {
  Suite* s = suite_create("strrchr");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strrchr_basic);
  tcase_add_test(tc, test_strrchr_edge);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_strstr(void) {
  Suite* s = suite_create("strstr");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strstr_found);
  tcase_add_test(tc, test_strstr_repeat);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_strtok(void) {
  Suite* s = suite_create("strtok");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_strtok_basic);
  tcase_add_test(tc, test_strtok_leading_trailing);
  tcase_add_test(tc, test_strtok_multi_delim);
  tcase_add_test(tc, test_strtok_edge);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_sprintf(void) {
  Suite* s = suite_create("sprintf");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_sprintf_char);
  tcase_add_test(tc, test_sprintf_int_basic);
  tcase_add_test(tc, test_sprintf_int_flags);
  tcase_add_test(tc, test_sprintf_int_length);
  tcase_add_test(tc, test_sprintf_float_flags);
  tcase_add_test(tc, test_sprintf_string_basic);
  tcase_add_test(tc, test_sprintf_unsigned);
  tcase_add_test(tc, test_sprintf_percent);
  tcase_add_test(tc, test_sprintf_all);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_trim(void) {
  Suite* s = suite_create("trim");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_trim_spaces);
  tcase_add_test(tc, test_trim_custom_chars);
  tcase_add_test(tc, test_trim_whitespace);
  tcase_add_test(tc, test_trim_edge);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_toupper(void) {
  Suite* s = suite_create("toupper");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_toupper_basic);
  tcase_add_test(tc, test_toupper_mixed);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_tolower(void) {
  Suite* s = suite_create("tolower");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_tolower_basic);
  tcase_add_test(tc, test_tolower_mixed);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_insert(void) {
  Suite* s = suite_create("insert");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_insert_basic);
  tcase_add_test(tc, test_insert_empty);
  tcase_add_test(tc, test_insert_invalid);
  suite_add_tcase(s, tc);
  return s;
}

static Suite* suite_sscanf(void) {
  Suite* s = suite_create("sscanf");
  TCase* tc = tcase_create("core");
  tcase_add_test(tc, test_sscanf_int_basic);
  tcase_add_test(tc, test_sscanf_int_width);
  tcase_add_test(tc, test_sscanf_int_length);
  tcase_add_test(tc, test_sscanf_i_auto_base);
  tcase_add_test(tc, test_sscanf_unsigned);
  tcase_add_test(tc, test_sscanf_octal);
  tcase_add_test(tc, test_sscanf_hex);
  tcase_add_test(tc, test_sscanf_float);
  tcase_add_test(tc, test_sscanf_float_long_double);
  tcase_add_test(tc, test_sscanf_char);
  tcase_add_test(tc, test_sscanf_string);
  tcase_add_test(tc, test_sscanf_string_width);
  tcase_add_test(tc, test_sscanf_multiple);
  tcase_add_test(tc, test_sscanf_suppress);
  tcase_add_test(tc, test_sscanf_n);

  tcase_add_test(tc, test_sscanf_d_basic);
  tcase_add_test(tc, test_sscanf_d_whitespace);
  tcase_add_test(tc, test_sscanf_d_width);
  tcase_add_test(tc, test_sscanf_d_length);
  tcase_add_test(tc, test_sscanf_d_overflow);

  tcase_add_test(tc, test_sscanf_i_width);

  tcase_add_test(tc, test_sscanf_u_basic);
  tcase_add_test(tc, test_sscanf_u_length);

  tcase_add_test(tc, test_sscanf_o_basic);

  tcase_add_test(tc, test_sscanf_p_basic);

  tcase_add_test(tc, test_sscanf_f_basic);
  tcase_add_test(tc, test_sscanf_f_scientific);
  tcase_add_test(tc, test_sscanf_f_double);
  tcase_add_test(tc, test_sscanf_f_long_double);
  tcase_add_test(tc, test_sscanf_f_width);
  tcase_add_test(tc, test_sscanf_f_invalid);

  tcase_add_test(tc, test_sscanf_c_width);

  tcase_add_test(tc, test_sscanf_s_basic);
  tcase_add_test(tc, test_sscanf_s_width);

  tcase_add_test(tc, test_sscanf_suppress);
  tcase_add_test(tc, test_sscanf_suppress_char);

  tcase_add_test(tc, test_sscanf_n_basic);
  tcase_add_test(tc, test_sscanf_multiple);
  tcase_add_test(tc, test_sscanf_mixed_bases);
  tcase_add_test(tc, test_sscanf_mixed_whitespace);

  suite_add_tcase(s, tc);
  return s;
}

static int run_suite(SRunner* sr, Suite* s) {
  srunner_add_suite(sr, s);
  return 0;
}

int main(void) {
  SRunner* sr = srunner_create(suite_memchr());

  run_suite(sr, suite_memcmp());
  run_suite(sr, suite_memcpy());
  run_suite(sr, suite_memset());
  run_suite(sr, suite_strncat());
  run_suite(sr, suite_strchr());
  run_suite(sr, suite_strncmp());
  run_suite(sr, suite_strncpy());
  run_suite(sr, suite_strcspn());
  run_suite(sr, suite_strerror());
  run_suite(sr, suite_strlen());
  run_suite(sr, suite_strpbrk());
  run_suite(sr, suite_strrchr());
  run_suite(sr, suite_strstr());
  run_suite(sr, suite_strtok());
  run_suite(sr, suite_sprintf());
  run_suite(sr, suite_trim());
  run_suite(sr, suite_toupper());
  run_suite(sr, suite_tolower());
  run_suite(sr, suite_insert());
  run_suite(sr, suite_sscanf());

  srunner_run_all(sr, CK_VERBOSE);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return ((failed == 0) ? 0 : 1);
}
