#include "s21_string.h"

int s21_memcmp(const void* ptr1, const void* ptr2, s21_size_t n) {
  const unsigned char* ptr_uchar1 = (const unsigned char*)ptr1;
  const unsigned char* ptr_uchar2 = (const unsigned char*)ptr2;

  int diff = 0;

  for (s21_size_t i = 0; diff == 0 && i < n; ++i) {
    if (ptr_uchar1[i] != ptr_uchar2[i]) {
      diff = ptr_uchar1[i] - ptr_uchar2[i];
    }
  }

  return diff;
}
