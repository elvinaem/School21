CFLAGS = -std=c11 -Wall -Werror -Wextra

all: s21_cat

s21_cat: s21_cat.c s21_cat.h
	gcc $(CFLAGS) s21_cat.c -o s21_cat

test: test.sh
	bash ./test.sh

clean:
	rm -f s21_cat *.o

rebuild: clean all

.PHONY: all clean rebuild

