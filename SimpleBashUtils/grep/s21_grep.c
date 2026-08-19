#include "s21_grep.h"

static int pattern(cont* cx, const char* pattern);
static int pattern_load(cont* cx, const char* filename);
static int check_arg(cont* cx, int optind, int argc, char* argv[]);
static void print_match(cont* cx, compil_patt* cp, const char* line,
                        const char* filename, int line_num);
static void context_free(cont* cx);

int main(int argc, char* argv[]) {
  cont cx = {0};
  compil_patt cp = {0};
  int stat = 0;

  if (parser(argc, argv, &cx) != 0) {
    stat = 1;
  }

  if (cx.flags.o && cx.flags.v) {
    return 0;
  }

  if (compil_patterns(&cx, &cp) != 0) {
    stat = 1;
  }

  if (stat == 0) {
    bool need_lines = !cx.flags.c && !cx.flags.l;

    if (cx.flags.c) {
      stat |= num_out_match(&cx, &cp);
    }
    if (cx.flags.l) {
      stat |= out_match_file(&cx, &cp);
    }
    if (need_lines) {
      stat |= out_match_row(&cx, &cp);
    }
  }

  compil_patt_free(&cp);
  context_free(&cx);

  return stat;
}

static void context_free(cont* cx) {
  if (cx && cx->patterns) {
    for (int i = 0; i < cx->patt_count; i++) {
      free(cx->patterns[i]);
    }
    free(cx->patterns);
    cx->patterns = NULL;
    cx->patt_count = 0;
  }
}

int num_out_match(cont* cx, compil_patt* cp) {
  int summa = 0;
  int file_valid = 0;
  for (int i = 0; i < cx->count_f; i++) {
    int count = count_match_rows(cx->files[i], cp, cx);
    if (count == -1) {
      continue;
    }
    if (cx->count_f > 1) {
      printf("%s:%d\n", cx->files[i], count);
    } else {
      summa += count;
      file_valid++;
    }
  }
  if (cx->count_f == 1 && file_valid > 0) {
    printf("%d\n", summa);
  }
  return 0;
}

int count_match_rows(const char* filename, compil_patt* cp, cont* cx) {
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    if (!cx->flags.s) perror(filename);
    return -1;
  }

  char line[LINE_MAX];
  int count = 0;
  while (fgets(line, sizeof(line), fp)) {
    if (match_pattern(line, cp, cx->flags.v)) {
      count++;
    }
  }
  fclose(fp);
  return count;
}

int parser(int argc, char* argv[], cont* cx) {
  int opt;
  while ((opt = getopt(argc, argv, "e:f:ivclnhso")) != -1) {
    switch (opt) {
      case 'e':
        cx->flags.e = true;
        if (pattern(cx, optarg) != 0) return 1;
        break;
      case 'i':
        cx->flags.i = true;
        break;
      case 'v':
        cx->flags.v = true;
        break;
      case 'c':
        cx->flags.c = true;
        break;
      case 'l':
        cx->flags.l = true;
        break;
      case 'n':
        cx->flags.n = true;
        break;
      case 'h':
        cx->flags.h = true;
        break;
      case 's':
        cx->flags.s = true;
        break;
      case 'f':
        cx->flags.f = true;
        if (pattern_load(cx, optarg) != 0) return 1;
        break;
      case 'o':
        cx->flags.o = true;
        break;
      case '?':
        fprintf(stderr, "Ошибка: %s [флаг]... паттерн [файл]...\n", argv[0]);
        return 1;
    }
  }

  if (check_arg(cx, optind, argc, argv) != 0) {
    return 1;
  }

  return 0;
}

static int pattern(cont* cx, const char* pattern) {
  char** tmp = realloc(cx->patterns, (cx->patt_count + 1) * sizeof(char*));
  if (!tmp) return 1;
  cx->patterns = tmp;
  cx->patterns[cx->patt_count] = strdup(pattern);
  if (!cx->patterns[cx->patt_count]) return 1;
  cx->patt_count++;
  return 0;
}

static int pattern_load(cont* cx, const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (!fp) {
    if (!cx->flags.s) perror(filename);
    return 1;
  }

  char line[LINE_MAX];
  while (fgets(line, sizeof(line), fp)) {
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
      line[len - 1] = '\0';
    }
    if (len == 0 || (len == 1 && line[0] == '\0')) continue;
    if (pattern(cx, line) != 0) {
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

static int check_arg(cont* cx, int optind, int argc, char* argv[]) {
  if (optind >= argc) {
    fprintf(stderr, "%s: файлы или паттерн не указаны\n", argv[0]);
    return 1;
  }

  if (!cx->flags.e && !cx->flags.f) {
    if (optind + 1 >= argc) {
      fprintf(stderr, "%s: файлы не указаны\n", argv[0]);
      return 1;
    }
    if (pattern(cx, argv[optind]) != 0) return 1;
    cx->files = &argv[optind + 1];
    cx->count_f = argc - optind - 1;
  } else {
    cx->files = &argv[optind];
    cx->count_f = argc - optind;
    if (cx->count_f == 0) {
      fprintf(stderr, "%s: файлы не указаны\n", argv[0]);
      return 1;
    }
  }
  return 0;
}

int out_match_row(cont* cx, compil_patt* cp) {
  for (int i = 0; i < cx->count_f; i++) {
    FILE* fp = fopen(cx->files[i], "r");
    if (!fp) {
      if (!cx->flags.s) perror(cx->files[i]);
      continue;
    }

    char line[LINE_MAX];
    int line_num = 1;

    while (fgets(line, sizeof(line), fp)) {
      if (cx->flags.o) {
        print_match(cx, cp, line, cx->files[i], line_num);
      } else {
        if (match_pattern(line, cp, cx->flags.v)) {
          if (cx->count_f > 1 && !cx->flags.h) {
            printf("%s:", cx->files[i]);
          }
          if (cx->flags.n) {
            printf("%d:", line_num);
          }
          printf("%s", line);
        }
      }
      line_num++;
    }
    fclose(fp);
  }
  return 0;
}

int out_match_file(cont* cx, compil_patt* cp) {
  for (int i = 0; i < cx->count_f; i++) {
    FILE* fp = fopen(cx->files[i], "r");
    if (!fp) {
      if (!cx->flags.s) {
        perror(cx->files[i]);
      }
      continue;
    }

    char line[LINE_MAX];
    bool matched = false;

    while (fgets(line, sizeof(line), fp)) {
      if (match_pattern(line, cp, cx->flags.v)) {
        matched = true;
        break;
      }
    }

    fclose(fp);
    if (matched) {
      printf("%s\n", cx->files[i]);
    }
  }
  return 0;
}

int compil_patterns(cont* cx, compil_patt* cp) {
  if (cx->patt_count == 0) {
    fprintf(stderr, "No patterns to compile\n");
    return 1;
  }

  cp->regexes = calloc(cx->patt_count, sizeof(regex_t));
  if (!cp->regexes) return 1;
  cp->count = cx->patt_count;

  int reg_flags = REG_EXTENDED;
  if (cx->flags.i) {
    reg_flags |= REG_ICASE;
  }

  for (int i = 0; i < cx->patt_count; i++) {
    if (regcomp(&cp->regexes[i], cx->patterns[i], reg_flags) != 0) {
      for (int j = 0; j < i; j++) {
        regfree(&cp->regexes[j]);
      }
      free(cp->regexes);
      cp->regexes = NULL;
      return 1;
    }
  }
  return 0;
}

void compil_patt_free(compil_patt* cp) {
  if (cp->regexes) {
    for (int i = 0; i < cp->count; i++) {
      regfree(&cp->regexes[i]);
    }
    free(cp->regexes);
    cp->regexes = NULL;
    cp->count = 0;
  }
}

bool match_pattern(const char* line, compil_patt* cp, bool invert) {
  bool found = false;
  for (int i = 0; i < cp->count; i++) {
    if (regexec(&cp->regexes[i], line, 0, NULL, 0) == 0) {
      found = true;
      break;
    }
  }
  return invert ? !found : found;
}

static void print_match(cont* cx, compil_patt* cp, const char* line,
                        const char* filename, int line_num) {
  bool matches = match_pattern(line, cp, false);
  bool must_print = cx->flags.v ? !matches : matches;

  if (cx->flags.v) {
    if (must_print) {
      if (cx->count_f > 1 && !cx->flags.h) {
        printf("%s:", filename);
      }
      if (cx->flags.n) {
        printf("%d:", line_num);
      }
      printf("%s", line);
    }
  } else {
    if (must_print) {
      for (int p = 0; p < cp->count; p++) {
        const char* current = line;
        regmatch_t correct_pos[1];

        while (regexec(&cp->regexes[p], current, 1, correct_pos, 0) == 0) {
          size_t start = correct_pos[0].rm_so;
          size_t end = correct_pos[0].rm_eo;

          if (start == (size_t)-1 || end == (size_t)-1) break;
          if (start == end) break;

          if (cx->count_f > 1 && !cx->flags.h) {
            printf("%s:", filename);
          }
          if (cx->flags.n) {
            printf("%d:", line_num);
          }

          fwrite(current + start, 1, end - start, stdout);
          printf("\n");

          current += end;

          if (end == 0) current++;
        }
      }
    }
  }
}