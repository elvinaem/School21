#include "s21_decimal.h"

int s21_from_decimal_to_int(s21_decimal src, int *dst) {
  int ret = 0;

  if (s21_validate_pointer(dst) || s21_decimal_is_invalid_format(src)) {
    ret = 1;
  } else {
    s21_decimal truncated;
    s21_truncate(src, &truncated);

    if (truncated.bits[1] != 0 || truncated.bits[2] != 0) {
      ret = 1;
    } else {
      int sign = s21_decimal_get_sign(src);
      unsigned int value = truncated.bits[0];

      if (sign && value > 2147483648U) {
        ret = 1;
      } else if (!sign && value > 2147483647U) {
        ret = 1;
      } else {
        *dst = sign ? -value : value;
      }
    }
  }

  return ret;
}
