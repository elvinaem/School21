#include "s21_cat.h"

arguments argument_parser(int argc, char **argv) {
  arguments arg = {0};

  struct option long_options[] = {{"number", no_argument, NULL, 'n'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {"number-nonblank", no_argument, NULL, 'b'},
                                  {0, 0, 0, 0}};

  int opt;

  while ((opt = getopt_long(argc, argv, "bnsvEetT", long_options, 0)) != -1) {
    switch (opt) {
      case 'b':
        arg.b = 1;
        arg.n = 0;
        break;

      case 'e':
        arg.E = 1;
        arg.v = 1;
        break;

      case 'n':
        if (!arg.b) {
          arg.n = 1;
        }
        break;

      case 's':
        arg.s = 1;
        break;

      case 't':
        arg.T = 1;
        arg.v = 1;
        break;

      case 'E':
        arg.E = 1;
        break;

      case 'T':
        arg.T = 1;
        break;

      case 'v':
        arg.v = 1;
        break;

      default:
        perror("Error option");
        break;
    }
  }
  return arg;
}

int is_non_printable(unsigned char c) { return ((c < 32) || c > 126); }

void v_output(unsigned char ch, arguments *arg) {
  if ((ch == '\t' && !arg->T) || ch == '\n') {
    putchar(ch);
  } else if (ch == 127) {
    printf("^?");
  } else {
    if (ch >= 128) {
      printf("M-");
      ch -= 128;
    }
    if (ch < 32) {
      printf("^");
      ch += 64;
    }
    putchar(ch);
  }
}

void outline(arguments *arg, char *line, int n) {
  for (int i = 0; i < n; ++i) {
    if (arg->E && line[i] == '\n') {
      putchar('$');
    }
    // putchar(line[i]);
    if (arg->v && is_non_printable(line[i])) {
      v_output(line[i], arg);
    } else if (arg->T && line[i] == '\t') {
      printf("^I");
    } else {
      putchar(line[i]);
    }
  }
}

void output(arguments *arg, char *argv[], int i, int *line_count,
            char *last_simbol_prev_str) {
  FILE *f = fopen(argv[i], "r");
  char *line = NULL;
  size_t numline = 0;
  int read = 0;

  int empty_count = 0;
  read = getline(&line, &numline, f);

  while (read != -1) {
    if (line[0] == '\n')
      empty_count++;
    else
      empty_count = 0;
    if (arg->n && *last_simbol_prev_str == '\n' &&
        !(arg->s && empty_count > 1)) {
      (*line_count)++;
    } else if (arg->b && empty_count == 0 && *last_simbol_prev_str == '\n') {
      (*line_count)++;
    }

    if (((arg->n || arg->b) && *last_simbol_prev_str == '\n') &&
        !(arg->s && empty_count > 1)) {
      if (arg->b && line[0] != '\n') {
        printf("%6d\t", *line_count);
      } else if (arg->n) {
        printf("%6d\t", *line_count);
      }
    }
    if (!(arg->s && empty_count > 1)) {
      outline(arg, line, read);
      *last_simbol_prev_str = line[read - 1];
    }

    read = getline(&line, &numline, f);
  }
  free(line);
  fclose(f);
}

void output_all_files(arguments *arg, int argc, char *argv[],
                      char *last_simbol_prev_str) {
  int line_count = 0;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      output(arg, argv, i, &line_count, last_simbol_prev_str);
    }
  }
}

int main(int argc, char *argv[]) {
  char last_simbol_prev_str = '\n';
  arguments arg = argument_parser(argc, argv);
  output_all_files(&arg, argc, argv, &last_simbol_prev_str);

  return 0;
}