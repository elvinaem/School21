
#include "s21_string.h"

long long int read_number_from_str(const char** str, int width,
                                   int* read_count) {
  long long int result = 0;
  int sign = 1;
  int count = 0;

  skip_whitespace(str);

  if (**str == '-' && (width < 0 || count < width)) {
    sign = -1;
    (*str)++;
    count++;
  } else if (**str == '+' && (width < 0 || count < width)) {
    (*str)++;
    count++;
  }

  while (**str && is_digit(**str) && (width < 0 || count < width)) {
    result = result * 10 + (**str - '0');
    (*str)++;
    count++;
  }

  if (read_count != S21_NULL) {
    *read_count = count;
  }

  return result * sign;
}

unsigned long long int read_octal_from_str(const char** str, int width,
                                           int* read_count) {
  unsigned long long int result = 0;
  int count = 0;

  skip_whitespace(str);

  while (**str && is_octal_digit(**str) && (width < 0 || count < width)) {
    result = result * 8 + (**str - '0');
    (*str)++;
    count++;
  }

  if (read_count != S21_NULL) {
    *read_count = count;
  }

  return result;
}

unsigned long long int read_hex_from_str(const char** str, int width,
                                         int* read_count) {
  unsigned long long int result = 0;
  int count = 0;

  skip_whitespace(str);

  if (**str == '0' && (*(*str + 1) == 'x' || *(*str + 1) == 'X')) {
    (*str) += 2;
    count += 2;
  }

  while (**str && is_hex_digit(**str) && (width < 0 || count < width)) {
    unsigned char c = **str;
    if (c >= '0' && c <= '9') {
      result = result * 16 + (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      result = result * 16 + (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      result = result * 16 + (c - 'A' + 10);
    }
    (*str)++;
    count++;
  }

  if (read_count != S21_NULL) {
    *read_count = count;
  }

  return result;
}

static int read_float_sign(const char** str, int width, int* count) {
  int sign = 1;
  if (**str == '-' && (width < 0 || *count < width)) {
    sign = -1;
    (*str)++;
    (*count)++;
  } else if (**str == '+' && (width < 0 || *count < width)) {
    (*str)++;
    (*count)++;
  }
  return sign;
}

static void read_float_integer_part(const char** str, int width, int* count,
                                    long long int* mantissa,
                                    int* mantissa_digits, int* extra_int_digits,
                                    int* had_digit) {
  while (**str && is_digit(**str) && (width < 0 || *count < width)) {
    if (*mantissa_digits < 18) {
      *mantissa = *mantissa * 10 + (**str - '0');
      (*mantissa_digits)++;
    } else {
      (*extra_int_digits)++;
    }
    (*str)++;
    (*count)++;
    *had_digit = 1;
  }
}

static void read_float_frac_part(const char** str, int width, int* count,
                                 long long int* mantissa, int* mantissa_digits,
                                 int* frac_digits, int* had_digit) {
  if (**str == '.' && (width < 0 || *count < width)) {
    (*str)++;
    (*count)++;
    while (**str && is_digit(**str) && (width < 0 || *count < width)) {
      if (*mantissa_digits < 18) {
        *mantissa = *mantissa * 10 + (**str - '0');
        (*mantissa_digits)++;
        (*frac_digits)++;
      }
      (*str)++;
      (*count)++;
      *had_digit = 1;
    }
  }
}

static int read_float_exp_part(const char** str, int width, int* count,
                               int* had_digit) {
  int exp_val = 0;
  if ((**str == 'e' || **str == 'E') && (width < 0 || *count < width)) {
    (*str)++;
    (*count)++;
    int exp_sign = 1;
    if (**str == '-' && (width < 0 || *count < width)) {
      exp_sign = -1;
      (*str)++;
      (*count)++;
    } else if (**str == '+' && (width < 0 || *count < width)) {
      (*str)++;
      (*count)++;
    }
    int exp_digit_count = 0;
    while (**str && is_digit(**str) && (width < 0 || *count < width)) {
      exp_val = exp_val * 10 + (**str - '0');
      (*str)++;
      (*count)++;
      exp_digit_count++;
    }
    if (exp_digit_count > 0) *had_digit = 1;
    exp_val *= exp_sign;
  }
  return exp_val;
}

long double read_float_from_str(const char** str, int width, int* read_count) {
  long long int mantissa = 0;
  int count = 0;
  int had_digit = 0;
  int mantissa_digits = 0;
  int extra_int_digits = 0;
  int frac_digits = 0;

  skip_whitespace(str);

  int sign = read_float_sign(str, width, &count);
  read_float_integer_part(str, width, &count, &mantissa, &mantissa_digits,
                          &extra_int_digits, &had_digit);
  read_float_frac_part(str, width, &count, &mantissa, &mantissa_digits,
                       &frac_digits, &had_digit);
  int exp_val = read_float_exp_part(str, width, &count, &had_digit);

  if (read_count != S21_NULL) {
    *read_count = had_digit ? count : 0;
  }

  long double result = 0.0L;
  if (had_digit) {
    long long int combined_exp =
        (long long int)exp_val - frac_digits + extra_int_digits;
    if (combined_exp >= 0) {
      result = (long double)mantissa * powl(10.0L, (long double)combined_exp);
    } else {
      result =
          (long double)mantissa / powl(10.0L, (long double)(-combined_exp));
    }
    result *= (long double)sign;
  }

  return result;
}

void handle_sscanf_char(SscanfInfo* info, const char** str, va_list* args,
                        int* assigned, int* chars_read) {
  int count = 0;
  int width = (info->width > 0) ? info->width : 1;

  if (info->suppress) {
    for (int i = 0; i < width && **str != '\0'; i++) {
      (*str)++;
      count++;
    }
  } else {
    char* ptr = va_arg(*args, char*);
    for (int i = 0; i < width && **str != '\0'; i++) {
      ptr[i] = **str;
      (*str)++;
      count++;
    }
    (*assigned)++;
  }

  *chars_read += count;
}

void handle_sscanf_string(SscanfInfo* info, const char** str, va_list* args,
                          int* assigned, int* chars_read) {
  int count = 0;
  int width = info->width;

  skip_whitespace(str);

  if (info->suppress) {
    while (**str && !is_whitespace(**str) && (width < 0 || count < width)) {
      (*str)++;
      count++;
    }
  } else {
    char* ptr = va_arg(*args, char*);
    while (**str && !is_whitespace(**str) && (width < 0 || count < width)) {
      ptr[count] = **str;
      (*str)++;
      count++;
    }
    ptr[count] = '\0';
    (*assigned)++;
  }

  *chars_read += count;
}

void handle_sscanf_int(SscanfInfo* info, const char** str, va_list* args,
                       int* assigned, int* chars_read) {
  int read_count = 0;
  long long int value;

  if (info->specifier == 'i') {
    skip_whitespace(str);
    if (**str == '0') {
      if (*(*str + 1) == 'x' || *(*str + 1) == 'X') {
        value = (long long int)read_hex_from_str(str, info->width, &read_count);
      } else {
        value =
            (long long int)read_octal_from_str(str, info->width, &read_count);
      }
    } else {
      value = read_number_from_str(str, info->width, &read_count);
    }
  } else {
    value = read_number_from_str(str, info->width, &read_count);
  }

  if (read_count > 0 && !info->suppress) {
    if (info->length == LEN_HALF_HALF) {
      signed char* ptr = va_arg(*args, signed char*);
      *ptr = (signed char)value;
    } else if (info->length == LEN_HALF) {
      short int* ptr = va_arg(*args, short int*);
      *ptr = (short int)value;
    } else if (info->length == LEN_LONG) {
      long int* ptr = va_arg(*args, long int*);
      *ptr = (long int)value;
    } else if (info->length == LEN_LONG_LONG) {
      long long int* ptr = va_arg(*args, long long int*);
      *ptr = value;
    } else {
      int* ptr = va_arg(*args, int*);
      *ptr = (int)value;
    }
    (*assigned)++;
  }

  *chars_read += read_count;
}

void handle_sscanf_unsigned(SscanfInfo* info, const char** str, va_list* args,
                            int* assigned, int* chars_read) {
  int read_count = 0;
  unsigned long long int value = (unsigned long long int)read_number_from_str(
      str, info->width, &read_count);

  if (read_count > 0 && !info->suppress) {
    if (info->length == LEN_HALF_HALF) {
      unsigned char* ptr = va_arg(*args, unsigned char*);
      *ptr = (unsigned char)value;
    } else if (info->length == LEN_HALF) {
      unsigned short int* ptr = va_arg(*args, unsigned short int*);
      *ptr = (unsigned short int)value;
    } else if (info->length == LEN_LONG) {
      unsigned long int* ptr = va_arg(*args, unsigned long int*);
      *ptr = (unsigned long int)value;
    } else if (info->length == LEN_LONG_LONG) {
      unsigned long long int* ptr = va_arg(*args, unsigned long long int*);
      *ptr = value;
    } else {
      unsigned int* ptr = va_arg(*args, unsigned int*);
      *ptr = (unsigned int)value;
    }
    (*assigned)++;
  }

  *chars_read += read_count;
}

void handle_sscanf_octal(SscanfInfo* info, const char** str, va_list* args,
                         int* assigned, int* chars_read) {
  int read_count = 0;
  unsigned long long int value =
      read_octal_from_str(str, info->width, &read_count);

  if (read_count > 0 && !info->suppress) {
    if (info->length == LEN_HALF_HALF) {
      unsigned char* ptr = va_arg(*args, unsigned char*);
      *ptr = (unsigned char)value;
    } else if (info->length == LEN_HALF) {
      unsigned short int* ptr = va_arg(*args, unsigned short int*);
      *ptr = (unsigned short int)value;
    } else if (info->length == LEN_LONG) {
      unsigned long int* ptr = va_arg(*args, unsigned long int*);
      *ptr = (unsigned long int)value;
    } else if (info->length == LEN_LONG_LONG) {
      unsigned long long int* ptr = va_arg(*args, unsigned long long int*);
      *ptr = value;
    } else {
      unsigned int* ptr = va_arg(*args, unsigned int*);
      *ptr = (unsigned int)value;
    }
    (*assigned)++;
  }

  *chars_read += read_count;
}

void handle_sscanf_hex(SscanfInfo* info, const char** str, va_list* args,
                       int* assigned, int* chars_read) {
  int read_count = 0;
  unsigned long long int value =
      read_hex_from_str(str, info->width, &read_count);

  if (read_count > 0 && !info->suppress) {
    if (info->length == LEN_HALF_HALF) {
      unsigned char* ptr = va_arg(*args, unsigned char*);
      *ptr = (unsigned char)value;
    } else if (info->length == LEN_HALF) {
      unsigned short int* ptr = va_arg(*args, unsigned short int*);
      *ptr = (unsigned short int)value;
    } else if (info->length == LEN_LONG) {
      unsigned long int* ptr = va_arg(*args, unsigned long int*);
      *ptr = (unsigned long int)value;
    } else if (info->length == LEN_LONG_LONG) {
      unsigned long long int* ptr = va_arg(*args, unsigned long long int*);
      *ptr = value;
    } else {
      unsigned int* ptr = va_arg(*args, unsigned int*);
      *ptr = (unsigned int)value;
    }
    (*assigned)++;
  }

  *chars_read += read_count;
}

void handle_sscanf_pointer(SscanfInfo* info, const char** str, va_list* args,
                           int* assigned, int* chars_read) {
  int read_count = 0;
  unsigned long long int value =
      read_hex_from_str(str, info->width, &read_count);

  if (read_count > 0 && !info->suppress) {
    void** ptr = va_arg(*args, void**);
    *ptr = (void*)(unsigned long long)value;
    (*assigned)++;
  }

  *chars_read += read_count;
}

void handle_sscanf_float(SscanfInfo* info, const char** str, va_list* args,
                         int* assigned, int* chars_read) {
  int read_count = 0;
  long double value = read_float_from_str(str, info->width, &read_count);

  if (read_count > 0 && !info->suppress) {
    if (info->length == LEN_L) {
      long double* ptr = va_arg(*args, long double*);
      *ptr = value;
    } else if (info->length == LEN_LONG) {
      double* ptr = va_arg(*args, double*);
      *ptr = (double)value;
    } else {
      float* ptr = va_arg(*args, float*);
      *ptr = (float)value;
    }
    (*assigned)++;
  }

  *chars_read += read_count;
}

void handle_sscanf_percent(const char** str, int* chars_read) {
  skip_whitespace(str);
  if (**str == '%') {
    (*str)++;
    (*chars_read)++;
  }
}

void handle_sscanf_count(SscanfInfo* info, va_list* args, int* chars_read) {
  if (!info->suppress) {
    int* ptr = va_arg(*args, int*);
    if (ptr != S21_NULL) {
      *ptr = *chars_read;
    }
  }
}

void handle_sscanf_specifier(SscanfInfo* info, const char** str, va_list* args,
                             int* assigned, int* chars_read) {
  if (info->specifier == 'c') {
    handle_sscanf_char(info, str, args, assigned, chars_read);
  } else if (info->specifier == 's') {
    handle_sscanf_string(info, str, args, assigned, chars_read);
  } else if (info->specifier == 'd' || info->specifier == 'i') {
    handle_sscanf_int(info, str, args, assigned, chars_read);
  } else if (info->specifier == 'u') {
    handle_sscanf_unsigned(info, str, args, assigned, chars_read);
  } else if (info->specifier == 'o') {
    handle_sscanf_octal(info, str, args, assigned, chars_read);
  } else if (info->specifier == 'x' || info->specifier == 'X') {
    handle_sscanf_hex(info, str, args, assigned, chars_read);
  } else if (info->specifier == 'p') {
    handle_sscanf_pointer(info, str, args, assigned, chars_read);
  } else if (info->specifier == 'e' || info->specifier == 'E' ||
             info->specifier == 'f' || info->specifier == 'g' ||
             info->specifier == 'G') {
    handle_sscanf_float(info, str, args, assigned, chars_read);
  } else if (info->specifier == '%') {
    handle_sscanf_percent(str, chars_read);
  } else if (info->specifier == 'n') {
    handle_sscanf_count(info, args, chars_read);
  }
}

static void parse_sscanf_spec(SscanfInfo* info, const char** format,
                              const char** str, va_list* args, int* assigned,
                              int* chars_read) {
  if (**format == '*') {
    info->suppress = 1;
    (*format)++;
  }
  if (is_width(**format) && **format != '*') {
    info->width = read_int_from_str(format);
  }
  while (is_length(**format)) {
    length_int(&info->length, **format);
    (*format)++;
  }
  if (is_specifier(**format)) {
    info->specifier = **format;
    (*format)++;
    handle_sscanf_specifier(info, str, args, assigned, chars_read);
  }
}

int s21_sscanf(const char* str, const char* format, ...) {
  va_list args;
  va_start(args, format);

  int assigned = 0;
  int chars_read = 0;
  int error = 0;
  SscanfInfo sscanf_info;
  reset_sscanf_info(&sscanf_info);

  while (*format && !error) {
    unsigned char c = *format;
    if (is_whitespace(c)) {
      while (is_whitespace(*format)) format++;
      const char* str_before_ws = str;
      skip_whitespace(&str);
      chars_read += (int)(str - str_before_ws);
    } else if (c != '%') {
      const char* str_before_lit = str;
      skip_whitespace(&str);
      chars_read += (int)(str - str_before_lit);
      if (*str == c) {
        str++;
        format++;
        chars_read++;
      } else {
        error = 1;
      }
    } else {
      format++;
      reset_sscanf_info(&sscanf_info);
      parse_sscanf_spec(&sscanf_info, &format, &str, &args, &assigned,
                        &chars_read);
    }
  }

  va_end(args);
  return assigned;
}