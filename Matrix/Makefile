CC = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11 -I.
GCOV_FLAGS = --coverage -fprofile-arcs -ftest-coverage

LIB_NAME = s21_matrix.a

LIB_SRCS = \
	s21_create_matrix.c \
	s21_remove_matrix.c \
	s21_eq_matrix.c \
	s21_sum_matrix.c \
	s21_sub_matrix.c \
	s21_mult_number.c \
	s21_mult_matrix.c \
	s21_transpose.c \
	s21_calc_complements.c \
	s21_determinant.c \
	s21_inverse_matrix.c \
	s21_helper_fun.c

LIB_OBJS = $(LIB_SRCS:.c=.o)

TEST_SRC = tests/s21_matrix_test.c
TEST_BIN = s21_test

CHECK_CFLAGS := $(shell pkg-config --cflags check 2>/dev/null)
CHECK_LIBS   := $(shell pkg-config --libs   check 2>/dev/null)
ifeq ($(CHECK_LIBS),)
    CHECK_CFLAGS =
    CHECK_LIBS   = -lcheck -lsubunit -lm -lpthread
endif

.PHONY: all clean test gcov_report rebuild install_deps

all: $(LIB_NAME)

$(LIB_NAME): $(LIB_OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_SRC) $(LIB_NAME)
	$(CC) $(CFLAGS) -Wl,--wrap=calloc $(CHECK_CFLAGS) $^ -o $@ $(CHECK_LIBS) 

test: $(TEST_BIN)
	./$(TEST_BIN)

gcov_report: $(TEST_SRC) $(LIB_SRCS)
	rm -f gcov_test gcov_test.exe *.gcda *.gcno coverage.info
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(CHECK_CFLAGS) \
		-Wl,--wrap=calloc $(TEST_SRC) $(LIB_SRCS) -o gcov_test $(CHECK_LIBS)
	./gcov_test
	lcov --capture --directory . --output-file coverage.info \
		--exclude '*test*'
	genhtml coverage.info --output-directory gcov_report
	@echo "Coverage report: gcov_report/index.html"

install_deps:
	sudo apt-get update && sudo apt-get install -y check libsubunit-dev lcov

format:
	clang-format *.c *.h tests/s21_matrix_test.c -i
	
clean:
	rm -f $(LIB_OBJS) $(LIB_NAME) $(TEST_BIN) debug_test debug_test.exe
	rm -f *.gcno *.gcda *.gcov gcov_test coverage.info
	rm -rf gcov_report

rebuild: clean all
