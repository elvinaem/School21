CC = gcc
CFLAGS  = -Wall -Wextra -Werror -std=c11
GCOV_FLAGS = --coverage -fprofile-arcs -ftest-coverage

LIB_NAME = s21_string.a

LIB_SRCS = \
	s21_memchr.c \
	s21_memcmp.c \
	s21_memcpy.c \
	s21_memset.c \
	s21_strncat.c \
	s21_strchr.c \
	s21_strncmp.c \
	s21_strncpy.c \
	s21_strcspn.c \
	s21_strerror.c \
	s21_strlen.c \
	s21_strpbrk.c \
	s21_strrchr.c \
	s21_strstr.c \
	s21_strtok.c \
	s21_trim.c \
	s21_insert.c \
	s21_tolower.c \
	s21_toupper.c \
	s21_format_utils.c \
	s21_sprintf.c \
	s21_sscanf.c

LIB_OBJS = $(LIB_SRCS:.c=.o)

TEST_SRC = s21_string_test.c
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
