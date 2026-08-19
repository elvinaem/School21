CC = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11
GCOV_FLAGS = --coverage -fprofile-arcs -ftest-coverage

LIB_NAME = s21_decimal.a

LIB_SRCS = \
	s21_basis_oper.c \
	s21_add.c \
	s21_sub.c \
	s21_mul.c \
	s21_div.c \
	s21_from_decimal_to_float.c \
	s21_from_decimal_to_int.c \
	s21_from_float_to_decimal.c \
	s21_from_int_to_decimal.c \
	s21_cmp_operators.c \
	s21_round_functions.c \
	s21_truncate.c \
	s21_negate.c \
	s21_helper_fun.c \
	s21_validate.c \

LIB_OBJS = $(LIB_SRCS:.c=.o)

TEST_SRC = s21_decimal_test.c
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
	$(CC) $(CFLAGS) $(CHECK_CFLAGS) $^ -o $@ $(CHECK_LIBS) 

test: $(TEST_BIN)
	./$(TEST_BIN)

gcov_report: $(TEST_SRC) $(LIB_SRCS)
	rm -f gcov_test gcov_test.exe *.gcda *.gcno coverage.info
	$(CC) $(CFLAGS) $(GCOV_FLAGS) $(CHECK_CFLAGS) \
		$(TEST_SRC) $(LIB_SRCS) -o gcov_test $(CHECK_LIBS)
	./gcov_test
	lcov --capture --directory . --output-file coverage.info \
		--exclude '*test*'
	genhtml coverage.info --output-directory gcov_report
	@echo "Coverage report: gcov_report/index.html"

install_deps:
	sudo apt-get update && sudo apt-get install -y check libsubunit-dev lcov

format:
	clang-format *.c *.h -i
	
clean:
	rm -f $(LIB_OBJS) $(LIB_NAME) $(TEST_BIN) debug_test debug_test.exe
	rm -f *.gcno *.gcda *.gcov gcov_test coverage.info
	rm -rf gcov_report

rebuild: clean all