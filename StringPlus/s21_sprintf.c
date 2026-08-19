
#include "s21_string.h"

int is_flag(unsigned char c) {
  return (c == '-' || c == '+' || c == ' ' || c == '#' || c == '0');
}

void reset_info(Info* info) {
  info->width = -1;
  info->precision = -1;
  info->length = LEN_NONE;
  info->specifier = '\0';
  info->flag_minus = 0;
  info->flag_plus = 0;
  info->flag_space = 0;
  info->flag_hash = 0;
  info->flag_zero = 0;
}

int s21_itoa(long long int value, char* str, int base, int uppercase) {
  int i = 0;
  int is_negative = 0;

  if (value < 0 && base == 10) {
    is_negative = 1;
    value = -value;
  }

  if (value == 0) {
    str[i++] = '0';
  } else {
    long long int tmp = value;
    int digits = 0;
    while (tmp > 0) {
      digits++;
      tmp /= base;
    }
    i = digits;
    str[digits] = '\0';
    while (digits > 0) {
      int digit = value % base;
      if (digit < 10) {
        str[--digits] = '0' + digit;
      } else {
        str[--digits] = (uppercase ? 'A' : 'a') + (digit - 10);
      }
      value /= base;
    }
  }

  if (is_negative) {
    for (int j = i; j >= 0; j--) {
      str[j + 1] = str[j];
    }
    str[0] = '-';
    i++;
  }

  return i;
}

int s21_utoa(unsigned long long int value, char* str, int base, int uppercase) {
  int i = 0;

  if (value == 0) {
    str[i++] = '0';
    str[i] = '\0';
  } else {
    unsigned long long int tmp = value;
    int digits = 0;
    while (tmp > 0) {
      digits++;
      tmp /= base;
    }
    i = digits;
    str[digits] = '\0';
    while (digits > 0) {
      int digit = value % base;
      if (digit < 10) {
        str[--digits] = '0' + digit;
      } else {
        str[--digits] = (uppercase ? 'A' : 'a') + (digit - 10);
      }
      value /= base;
    }
  }

  return i;
}

int s21_ftoa(long double value, char* str, Info* info) {
  int i = 0;
  int precision = (info->precision >= 0) ? info->precision : 6;

  if (value < 0) {
    str[i++] = '-';
    value = -value;
  }

  unsigned long long int int_part = (unsigned long long int)value;
  long double frac_part = value - int_part;

  char int_str[64];
  int int_len = s21_utoa(int_part, int_str, 10, 0);
  for (int j = 0; j < int_len; j++) {
    str[i++] = int_str[j];
  }

  if (precision > 0 || info->flag_hash) {
    str[i++] = '.';

    if (precision > 0) {
      long double multiplier = 1.0;
      for (int j = 0; j < precision; j++) {
        multiplier *= 10.0;
      }

      unsigned long long int frac_int =
          (unsigned long long int)(frac_part * multiplier + 0.5);

      char frac_str[64];
      int frac_len = s21_utoa(frac_int, frac_str, 10, 0);

      for (int j = 0; j < precision - frac_len; j++) {
        str[i++] = '0';
      }

      for (int j = 0; j < frac_len; j++) {
        str[i++] = frac_str[j];
      }
    }
  }

  str[i] = '\0';
  return i;
}

int s21_etoa(long double value, char* str, Info* info, int uppercase) {
  int i = 0;
  int precision = (info->precision >= 0) ? info->precision : 6;
  int exp = 0;

  if (value < 0) {
    str[i++] = '-';
    value = -value;
  }

  if (value != 0.0) {
    exp = (int)floorl(log10l(value));
    value /= powl(10.0L, (long double)exp);
    if (value >= 10.0L) {
      value /= 10.0L;
      exp++;
    } else if (value < 1.0L) {
      value *= 10.0L;
      exp--;
    }
  }

  Info temp_info = *info;
  temp_info.precision = precision;
  i += s21_ftoa(value, str + i, &temp_info);

  str[i++] = uppercase ? 'E' : 'e';
  str[i++] = (exp >= 0) ? '+' : '-';

  int abs_exp = (exp >= 0) ? exp : -exp;
  if (abs_exp < 10) {
    str[i++] = '0';
  }

  char exp_str[8];
  int exp_len = s21_itoa(abs_exp, exp_str, 10, 0);
  for (int j = 0; j < exp_len; j++) {
    str[i++] = exp_str[j];
  }

  str[i] = '\0';
  return i;
}

static int is_numeric_unsigned(char spec) {
  return (spec == 'u' || spec == 'o' || spec == 'x' || spec == 'X');
}

static char get_sign_char(Info* info, int is_negative) {
  char sign = '\0';
  if (is_negative) {
    sign = '-';
  } else if (info->flag_plus && !is_numeric_unsigned(info->specifier)) {
    sign = '+';
  } else if (info->flag_space && !info->flag_plus &&
             !is_numeric_unsigned(info->specifier)) {
    sign = ' ';
  }
  return sign;
}

static int calc_effective_width(Info* info, int len, int is_negative) {
  int width = info->width;
  if (width <= len) {
    char sign = get_sign_char(info, is_negative);
    width = len + (sign != '\0' ? 1 : 0);
  }
  return width;
}

static int prepend_zero_padding(char* result, int pos, Info* info,
                                int is_negative, int padding_len) {
  char sign = get_sign_char(info, is_negative);
  if (sign != '\0') {
    result[pos++] = sign;
    padding_len--;
  }
  for (int j = 0; j < padding_len; j++) {
    result[pos++] = '0';
  }
  return pos;
}

static int prepend_space_padding(char* result, int pos, Info* info,
                                 int is_negative, int padding_len) {
  char sign = get_sign_char(info, is_negative);
  int sign_needed = (sign != '\0') ? 1 : 0;
  for (int j = 0; j < padding_len - sign_needed; j++) {
    result[pos++] = ' ';
  }
  return pos;
}

static int prepend_padding(char* result, int pos, Info* info, int is_negative,
                           int width, int len) {
  int sign_printed = 0;
  int padding_len = width - len;
  if (!info->flag_minus && padding_len > 0) {
    char padding = (info->flag_zero && !info->flag_minus) ? '0' : ' ';
    if (padding == '0') {
      pos = prepend_zero_padding(result, pos, info, is_negative, padding_len);
      sign_printed = 1;
    } else {
      pos = prepend_space_padding(result, pos, info, is_negative, padding_len);
    }
  }
  char sign = get_sign_char(info, is_negative);
  if (!sign_printed && sign != '\0') {
    result[pos++] = sign;
  }
  return pos;
}

void apply_width_and_flags(char* buffer, Info* info, int len, int is_negative) {
  char sign = get_sign_char(info, is_negative);
  if (info->width > len || sign != '\0') {
    int width = calc_effective_width(info, len, is_negative);
    char result[1024] = {0};
    int pos = prepend_padding(result, 0, info, is_negative, width, len);

    for (int j = 0; j < len; j++) {
      result[pos++] = buffer[j];
    }

    if (info->flag_minus) {
      int padding_len = width - pos;
      for (int j = 0; j < padding_len; j++) {
        result[pos++] = ' ';
      }
    }

    result[pos] = '\0';
    for (int j = 0; j <= pos; j++) {
      buffer[j] = result[j];
    }
  }
}

static void write_buffer_to_str(char* buffer, char** str, int* written) {
  int i = 0;
  while (buffer[i] != '\0') {
    **str = buffer[i];
    (*str)++;
    (*written)++;
    i++;
  }
}

static int apply_precision_signed(char* buffer, int len, int precision,
                                  int is_negative) {
  int result = len;
  if (precision > len) {
    char temp[128] = {0};
    int pos = 0;
    if (is_negative) temp[pos++] = '-';
    for (int i = 0; i < precision - len; i++) temp[pos++] = '0';
    int start = is_negative ? 1 : 0;
    for (int i = start; i < len + start; i++) temp[pos++] = buffer[i];
    temp[pos] = '\0';
    for (int i = 0; i <= pos; i++) buffer[i] = temp[i];
    result = pos;
  }
  return result;
}

static int apply_precision_unsigned(char* buffer, int len, int precision) {
  int result = len;
  if (precision > len) {
    char temp[128] = {0};
    int pos = 0;
    for (int i = 0; i < precision - len; i++) temp[pos++] = '0';
    for (int i = 0; i < len; i++) temp[pos++] = buffer[i];
    temp[pos] = '\0';
    for (int i = 0; i <= pos; i++) buffer[i] = temp[i];
    result = pos;
  }
  return result;
}

void handle_char(Info* info, char** str, va_list* args, int* written) {
  char c = (char)va_arg(*args, int);

  if (info->width > 1 && !info->flag_minus) {
    for (int i = 0; i < info->width - 1; i++) {
      **str = ' ';
      (*str)++;
      (*written)++;
    }
  }

  **str = c;
  (*str)++;
  (*written)++;

  if (info->width > 1 && info->flag_minus) {
    for (int i = 0; i < info->width - 1; i++) {
      **str = ' ';
      (*str)++;
      (*written)++;
    }
  }
}

void handle_string(Info* info, char** str, va_list* args, int* written) {
  char* s;
  if (info->length == LEN_LONG) {
    s = (char*)va_arg(*args, char*);
  } else {
    s = va_arg(*args, char*);
  }

  if (s == S21_NULL) {
    s = "(null)";
  }

  int len = 0;
  while (s[len] != '\0' && (info->precision < 0 || len < info->precision)) {
    len++;
  }

  if (info->width > len && !info->flag_minus) {
    for (int i = 0; i < info->width - len; i++) {
      **str = ' ';
      (*str)++;
      (*written)++;
    }
  }

  for (int i = 0; i < len; i++) {
    **str = s[i];
    (*str)++;
    (*written)++;
  }

  if (info->width > len && info->flag_minus) {
    for (int i = 0; i < info->width - len; i++) {
      **str = ' ';
      (*str)++;
      (*written)++;
    }
  }
}

void handle_int(Info* info, char** str, va_list* args, int* written) {
  long long int value;

  if (info->length == LEN_HALF_HALF) {
    value = (char)va_arg(*args, int);
  } else if (info->length == LEN_HALF) {
    value = (short int)va_arg(*args, int);
  } else if (info->length == LEN_LONG) {
    value = va_arg(*args, long int);
  } else if (info->length == LEN_LONG_LONG) {
    value = va_arg(*args, long long int);
  } else {
    value = va_arg(*args, int);
  }

  int is_negative = (value < 0);
  char buffer[128] = {0};

  int len;
  if (info->precision == 0 && value == 0) {
    buffer[0] = '\0';
    len = 0;
  } else {
    if (is_negative && value != -9223372036854775807LL - 1) {
      value = -value;
    }
    len = s21_itoa(value, buffer, 10, 0);
  }

  len = apply_precision_signed(buffer, len, info->precision, is_negative);
  apply_width_and_flags(buffer, info, len, is_negative);
  write_buffer_to_str(buffer, str, written);
}

void handle_unsigned(Info* info, char** str, va_list* args, int* written) {
  unsigned long long int value;

  if (info->length == LEN_HALF_HALF) {
    value = (unsigned char)va_arg(*args, unsigned int);
  } else if (info->length == LEN_HALF) {
    value = (unsigned short int)va_arg(*args, unsigned int);
  } else if (info->length == LEN_LONG) {
    value = va_arg(*args, unsigned long int);
  } else if (info->length == LEN_LONG_LONG) {
    value = va_arg(*args, unsigned long long int);
  } else {
    value = va_arg(*args, unsigned int);
  }

  char buffer[128] = {0};

  int len;
  if (info->precision == 0 && value == 0) {
    buffer[0] = '\0';
    len = 0;
  } else {
    len = s21_utoa(value, buffer, 10, 0);
  }

  len = apply_precision_unsigned(buffer, len, info->precision);
  apply_width_and_flags(buffer, info, len, 0);
  write_buffer_to_str(buffer, str, written);
}

void handle_octal(Info* info, char** str, va_list* args, int* written) {
  unsigned long long int value;

  if (info->length == LEN_HALF_HALF) {
    value = (unsigned char)va_arg(*args, unsigned int);
  } else if (info->length == LEN_HALF) {
    value = (unsigned short int)va_arg(*args, unsigned int);
  } else if (info->length == LEN_LONG) {
    value = va_arg(*args, unsigned long int);
  } else if (info->length == LEN_LONG_LONG) {
    value = va_arg(*args, unsigned long long int);
  } else {
    value = va_arg(*args, unsigned int);
  }

  char buffer[128] = {0};

  int len;
  if (info->precision == 0 && value == 0 && !info->flag_hash) {
    buffer[0] = '\0';
    len = 0;
  } else {
    len = s21_utoa(value, buffer, 8, 0);
  }

  if (info->flag_hash && value != 0 && len > 0) {
    for (int i = len; i >= 0; i--) {
      buffer[i + 1] = buffer[i];
    }
    buffer[0] = '0';
    len++;
  }

  len = apply_precision_unsigned(buffer, len, info->precision);
  apply_width_and_flags(buffer, info, len, 0);
  write_buffer_to_str(buffer, str, written);
}

void handle_hex(Info* info, char** str, va_list* args, int* written) {
  unsigned long long int value;

  if (info->length == LEN_HALF_HALF) {
    value = (unsigned char)va_arg(*args, unsigned int);
  } else if (info->length == LEN_HALF) {
    value = (unsigned short int)va_arg(*args, unsigned int);
  } else if (info->length == LEN_LONG) {
    value = va_arg(*args, unsigned long int);
  } else if (info->length == LEN_LONG_LONG) {
    value = va_arg(*args, unsigned long long int);
  } else {
    value = va_arg(*args, unsigned int);
  }

  char buffer[128] = {0};
  int uppercase = (info->specifier == 'X');

  int len;
  if (info->precision == 0 && value == 0) {
    buffer[0] = '\0';
    len = 0;
  } else {
    len = s21_utoa(value, buffer, 16, uppercase);
  }

  if (info->flag_hash && value != 0) {
    for (int i = len; i >= 0; i--) {
      buffer[i + 2] = buffer[i];
    }
    buffer[0] = '0';
    buffer[1] = uppercase ? 'X' : 'x';
    len += 2;
  }

  if (info->precision > len) {
    int prefix_len = (info->flag_hash && value != 0) ? 2 : 0;
    char temp[128] = {0};
    int pos = 0;
    for (int i = 0; i < prefix_len; i++) temp[pos++] = buffer[i];
    for (int i = 0; i < info->precision - (len - prefix_len); i++)
      temp[pos++] = '0';
    for (int i = prefix_len; i < len; i++) temp[pos++] = buffer[i];
    temp[pos] = '\0';
    for (int i = 0; i <= pos; i++) buffer[i] = temp[i];
    len = pos;
  }

  apply_width_and_flags(buffer, info, len, 0);
  write_buffer_to_str(buffer, str, written);
}

void handle_pointer(Info* info, char** str, va_list* args, int* written) {
  void* ptr = va_arg(*args, void*);

  char buffer[128] = {0};
  int len = s21_utoa((unsigned long long int)ptr, buffer + 2, 16, 0);
  buffer[0] = '0';
  buffer[1] = 'x';
  len += 2;

  apply_width_and_flags(buffer, info, len, 0);
  write_buffer_to_str(buffer, str, written);
}

void handle_float(Info* info, char** str, va_list* args, int* written) {
  long double value;

  if (info->length == LEN_L) {
    value = va_arg(*args, long double);
  } else {
    value = (long double)va_arg(*args, double);
  }

  char buffer[1024] = {0};
  int len = s21_ftoa(value, buffer, info);

  apply_width_and_flags(buffer, info, len, (value < 0));
  write_buffer_to_str(buffer, str, written);
}

void handle_scientific(Info* info, char** str, va_list* args, int* written) {
  long double value;

  if (info->length == LEN_L) {
    value = va_arg(*args, long double);
  } else {
    value = (long double)va_arg(*args, double);
  }

  char buffer[1024] = {0};
  int uppercase = (info->specifier == 'E');
  int len = s21_etoa(value, buffer, info, uppercase);

  apply_width_and_flags(buffer, info, len, (value < 0));
  write_buffer_to_str(buffer, str, written);
}

static int find_exp_pos(const char* buf, int len) {
  int e_pos = -1;
  for (int i = 0; i < len; i++) {
    if (buf[i] == 'e' || buf[i] == 'E') {
      e_pos = i;
      break;
    }
  }
  return e_pos;
}

static int find_dot_pos(const char* buf, int end) {
  int dot_pos = -1;
  for (int i = 0; i < end; i++) {
    if (buf[i] == '.') {
      dot_pos = i;
      break;
    }
  }
  return dot_pos;
}

static void strip_trailing_zeros(char* buf, int* len) {
  int e_pos = find_exp_pos(buf, *len);
  int end = (e_pos >= 0) ? e_pos : *len;
  int dot_pos = find_dot_pos(buf, end);

  if (dot_pos >= 0) {
    int new_end = end;
    while (new_end > dot_pos + 1 && buf[new_end - 1] == '0') {
      new_end--;
    }
    if (new_end == dot_pos + 1) {
      new_end = dot_pos;
    }
    if (new_end < end) {
      int shift = end - new_end;
      for (int i = new_end; i < *len - shift; i++) {
        buf[i] = buf[i + shift];
      }
      *len -= shift;
      buf[*len] = '\0';
    }
  }
}

static int get_float_exp(long double abs_val) {
  int exp = 0;
  long double temp = abs_val;
  while (temp >= 10.0) {
    temp /= 10.0;
    exp++;
  }
  while (temp < 1.0) {
    temp *= 10.0;
    exp--;
  }
  return exp;
}

static int count_int_digits(unsigned long long int val) {
  int digits = 0;
  unsigned long long int tmp = (val == 0) ? 1 : val;
  while (tmp > 0) {
    digits++;
    tmp /= 10;
  }
  return digits;
}

static int format_shortest_value(Info* info, long double value,
                                 long double abs_val, int precision,
                                 char* buffer) {
  int use_scientific = 0;
  if (abs_val != 0.0) {
    int exp = get_float_exp(abs_val);
    if (exp < -4 || exp >= precision) use_scientific = 1;
  }

  int len;
  if (use_scientific) {
    info->precision = precision - 1;
    len = s21_etoa(value, buffer, info, (info->specifier == 'G'));
  } else {
    int int_digits = count_int_digits((unsigned long long int)abs_val);
    int decimal_prec = precision - int_digits;
    if (decimal_prec < 0) decimal_prec = 0;
    info->precision = decimal_prec;
    len = s21_ftoa(value, buffer, info);
  }
  return len;
}

void handle_shortest(Info* info, char** str, va_list* args, int* written) {
  long double value;

  if (info->length == LEN_L) {
    value = va_arg(*args, long double);
  } else {
    value = (long double)va_arg(*args, double);
  }

  int precision = (info->precision >= 0) ? info->precision : 6;
  long double abs_val = (value < 0) ? -value : value;

  char buffer[1024] = {0};
  int len = format_shortest_value(info, value, abs_val, precision, buffer);

  if (!info->flag_hash) strip_trailing_zeros(buffer, &len);

  apply_width_and_flags(buffer, info, len, (value < 0));

  len = 0;
  while (buffer[len] != '\0') {
    **str = buffer[len];
    (*str)++;
    (*written)++;
    len++;
  }
}

void handle_percent(Info* info, char** str, int* written) {
  if (info->width > 1 && !info->flag_minus) {
    char pad = info->flag_zero ? '0' : ' ';
    for (int i = 0; i < info->width - 1; i++) {
      **str = pad;
      (*str)++;
      (*written)++;
    }
  }

  **str = '%';
  (*str)++;
  (*written)++;

  if (info->width > 1 && info->flag_minus) {
    for (int i = 0; i < info->width - 1; i++) {
      **str = ' ';
      (*str)++;
      (*written)++;
    }
  }
}

void handle_count(va_list* args, int* written) {
  int* ptr = va_arg(*args, int*);
  if (ptr != S21_NULL) {
    *ptr = *written;
  }
}

void handle_specifier(Info* info, char** str, va_list* args, int* written) {
  if (info->specifier == 'c') {
    handle_char(info, str, args, written);
  } else if (info->specifier == 's') {
    handle_string(info, str, args, written);
  } else if (info->specifier == 'd' || info->specifier == 'i') {
    handle_int(info, str, args, written);
  } else if (info->specifier == 'u') {
    handle_unsigned(info, str, args, written);
  } else if (info->specifier == 'o') {
    handle_octal(info, str, args, written);
  } else if (info->specifier == 'x' || info->specifier == 'X') {
    handle_hex(info, str, args, written);
  } else if (info->specifier == 'p') {
    handle_pointer(info, str, args, written);
  } else if (info->specifier == 'f') {
    handle_float(info, str, args, written);
  } else if (info->specifier == 'e' || info->specifier == 'E') {
    handle_scientific(info, str, args, written);
  } else if (info->specifier == 'g' || info->specifier == 'G') {
    handle_shortest(info, str, args, written);
  } else if (info->specifier == '%') {
    handle_percent(info, str, written);
  } else if (info->specifier == 'n') {
    handle_count(args, written);
  }
}

void parser_flags(Info* info, int c) {
  if (c == '-') {
    info->flag_minus = 1;
  } else if (c == '+') {
    info->flag_plus = 1;
  } else if (c == ' ') {
    info->flag_space = 1;
  } else if (c == '#') {
    info->flag_hash = 1;
  } else if (c == '0') {
    info->flag_zero = 1;
  }
}

static void parse_width(Info* info, const char** format, va_list* args) {
  if (!is_width(**format)) return;
  if (**format == '*') {
    info->width = va_arg(*args, int);
    if (info->width < 0) {
      info->width = -info->width;
      info->flag_minus = 1;
    }
    (*format)++;
  } else {
    info->width = read_int_from_str(format);
  }
}

static void parse_precision(Info* info, const char** format, va_list* args) {
  if (**format != '.') return;
  (*format)++;
  if (**format == '*') {
    info->precision = va_arg(*args, int);
    if (info->precision < 0) info->precision = -1;
    (*format)++;
  } else {
    info->precision = read_int_from_str(format);
  }
}

static void parse_format_spec(Info* info, const char** format, char** str,
                              va_list* args, int* written) {
  while (**format && is_flag(**format)) {
    parser_flags(info, **format);
    (*format)++;
  }
  parse_width(info, format, args);
  parse_precision(info, format, args);
  while (is_length(**format)) {
    length_int(&info->length, **format);
    (*format)++;
  }
  if (is_specifier(**format)) {
    info->specifier = **format;
    (*format)++;
    handle_specifier(info, str, args, written);
  }
}

int s21_sprintf(char* str, const char* format, ...) {
  va_list args;
  va_start(args, format);

  int written = 0;
  Info sprintf_info;
  reset_info(&sprintf_info);

  while (*format) {
    unsigned char c = *format;
    if (c != '%') {
      *str = c;
      str++;
      format++;
      written++;
    } else {
      format++;
      reset_info(&sprintf_info);
      parse_format_spec(&sprintf_info, &format, &str, &args, &written);
    }
  }

  *str = '\0';
  va_end(args);
  return written;
}
