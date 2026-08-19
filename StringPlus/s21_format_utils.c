#include "s21_string.h"

int is_whitespace(unsigned char c) {
  return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
          c == '\r');
}

int is_digit(unsigned char c) { return (c >= '0' && c <= '9'); }

int is_octal_digit(unsigned char c) { return (c >= '0' && c <= '7'); }

int is_hex_digit(unsigned char c) {
  return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'));
}

void skip_whitespace(const char** str) {
  while (**str && is_whitespace(**str)) {
    (*str)++;
  }
}

int is_width(unsigned char c) { return (c == '*' || (c >= '0' && c <= '9')); }

int is_symbol_num(unsigned char c) { return (c >= '0' && c <= '9'); }

int is_length(unsigned char c) { return (c == 'l' || c == 'L' || c == 'h'); }

int is_specifier(unsigned char c) {
  return (c == 'c' || c == 'd' || c == 'i' || c == 'e' || c == 'E' ||
          c == 'f' || c == 'g' || c == 'G' || c == 'o' || c == 's' ||
          c == 'u' || c == 'x' || c == 'X' || c == 'p' || c == 'n' || c == '%');
}

int read_int_from_str(const char** format) {
  int res = 0;
  while (**format && is_symbol_num(**format)) {
    res = res * 10 + (**format - '0');
    (*format)++;
  }
  return res;
}

void length_int(LengthType* len_ptr, int c) {
  if (*len_ptr == LEN_NONE) {
    if (c == 'l') {
      *len_ptr = LEN_LONG;
    } else if (c == 'L') {
      *len_ptr = LEN_L;
    } else if (c == 'h') {
      *len_ptr = LEN_HALF;
    }
  } else if (*len_ptr == LEN_LONG && c == 'l') {
    *len_ptr = LEN_LONG_LONG;
  } else if (*len_ptr == LEN_HALF && c == 'h') {
    *len_ptr = LEN_HALF_HALF;
  }
}

void reset_sscanf_info(SscanfInfo* info) {
  info->width = -1;
  info->length = LEN_NONE;
  info->specifier = '\0';
  info->suppress = 0;
}
