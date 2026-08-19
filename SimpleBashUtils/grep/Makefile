
CFLAGS = -std=c11 -Wall -Werror -Wextra

all: s21_grep

s21_grep: s21_grep.c s21_grep.h
	gcc $(CFLAGS) s21_grep.c -o s21_grep

test: tests.sh
	./tests.sh

clean:
	rm -f s21_grep *.o

rebuild: clean all

.PHONY: all clean rebuild