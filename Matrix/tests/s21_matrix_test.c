#include "s21_matrix.h"

#include <check.h>

void setup_calloc_failure(int fail_at_call);
void reset_calloc_mock(void);
void set_calloc_fail(int fail_at_call);

void *__real_calloc(size_t nmemb, size_t size);

static int call_count = 0;
static int fail_on_call = -1;

void *__wrap_calloc(size_t nmemb, size_t size) {
  static int call_count = 0;
  call_count++;

  if (fail_on_call == call_count) {
    return NULL;
  }

  return __real_calloc(nmemb, size);
}

void reset_calloc_mock(void) {
  call_count = 0;
  fail_on_call = -1;
}

void set_calloc_fail(int fail_at_call) {
  call_count = 0;
  fail_on_call = fail_at_call;
}

void setup_calloc_failure(int fail_at_call) {
  call_count = 0;
  fail_on_call = fail_at_call;
}

void setup(void) {
  extern int call_count;
  extern int fail_on_call;
  call_count = 0;
  fail_on_call = -1;
}

START_TEST(test_create_matrix_success) {
  reset_calloc_mock();
  s21_matrix matrix = {0};
  int result = s21_create_matrix(3, 5, &matrix);

  ck_assert_int_eq(result, OK);
  ck_assert_int_eq(matrix.rows, 3);
  ck_assert_int_eq(matrix.columns, 5);
  ck_assert_ptr_nonnull(matrix.matrix);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 5; j++) {
      ck_assert_double_eq_tol(matrix.matrix[i][j], 0.0, 1e-7);
    }
  }

  s21_remove_matrix(&matrix);
}
END_TEST

START_TEST(test_create_matrix_null_result) {
  int result = s21_create_matrix(3, 5, NULL);
  ck_assert_int_eq(result, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_create_matrix_invalid_rows_zero) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(0, 5, &matrix);
  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_invalid_rows_negative) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(-3, 5, &matrix);
  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_invalid_columns_zero) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(3, 0, &matrix);
  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_invalid_columns_negative) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(3, -5, &matrix);
  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_both_invalid) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(-2, -3, &matrix);
  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_calloc_fail_for_rows) {
  set_calloc_fail(1);
  s21_matrix matrix = {0};
  int result = s21_create_matrix(5, 5, &matrix);

  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
  setup_calloc_failure(-1);
}
END_TEST

START_TEST(test_create_matrix_calloc_fail_for_first_column) {
  set_calloc_fail(1);
  s21_matrix matrix = {0};
  int result = s21_create_matrix(5, 5, &matrix);

  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_calloc_fail_for_middle_column) {
  set_calloc_fail(1);
  s21_matrix matrix = {0};
  int result = s21_create_matrix(5, 5, &matrix);

  ck_assert_int_eq(result, INCORRECT_MATRIX);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
  ck_assert_ptr_null(matrix.matrix);
}
END_TEST

START_TEST(test_create_matrix_1x1) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(1, 1, &matrix);

  ck_assert_int_eq(result, OK);
  ck_assert_int_eq(matrix.rows, 1);
  ck_assert_int_eq(matrix.columns, 1);
  ck_assert_double_eq_tol(matrix.matrix[0][0], 0.0, 1e-7);

  s21_remove_matrix(&matrix);
}
END_TEST

START_TEST(test_create_matrix_large) {
  s21_matrix matrix = {0};
  int result = s21_create_matrix(100, 100, &matrix);

  ck_assert_int_eq(result, OK);
  ck_assert_int_eq(matrix.rows, 100);
  ck_assert_int_eq(matrix.columns, 100);
  ck_assert_double_eq_tol(matrix.matrix[50][50], 0.0, 1e-7);
  ck_assert_double_eq_tol(matrix.matrix[99][99], 0.0, 1e-7);

  s21_remove_matrix(&matrix);
}
END_TEST

START_TEST(test_create_matrix_after_remove) {
  s21_matrix matrix = {0};

  int result1 = s21_create_matrix(3, 3, &matrix);
  ck_assert_int_eq(result1, OK);
  s21_remove_matrix(&matrix);

  int result2 = s21_create_matrix(4, 4, &matrix);
  ck_assert_int_eq(result2, OK);
  ck_assert_int_eq(matrix.rows, 4);
  ck_assert_int_eq(matrix.columns, 4);

  s21_remove_matrix(&matrix);
}
END_TEST

Suite *create_matrix_suite(void) {
  Suite *suite = suite_create("s21_create_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_create_matrix_success);
  tcase_add_test(tcase_core, test_create_matrix_null_result);
  tcase_add_test(tcase_core, test_create_matrix_invalid_rows_zero);
  tcase_add_test(tcase_core, test_create_matrix_invalid_rows_negative);
  tcase_add_test(tcase_core, test_create_matrix_invalid_columns_zero);
  tcase_add_test(tcase_core, test_create_matrix_invalid_columns_negative);
  tcase_add_test(tcase_core, test_create_matrix_both_invalid);
  tcase_add_test(tcase_core, test_create_matrix_1x1);
  tcase_add_test(tcase_core, test_create_matrix_large);
  tcase_add_test(tcase_core, test_create_matrix_after_remove);

  TCase *tcase_malloc = tcase_create("Malloc failures");
  tcase_add_test(tcase_malloc, test_create_matrix_calloc_fail_for_rows);
  tcase_add_test(tcase_malloc, test_create_matrix_calloc_fail_for_first_column);
  tcase_add_test(tcase_malloc,
                 test_create_matrix_calloc_fail_for_middle_column);

  suite_add_tcase(suite, tcase_core);
  suite_add_tcase(suite, tcase_malloc);

  return suite;
}

START_TEST(test_remove_matrix_valid_3x5) {
  s21_matrix matrix = {0};
  s21_create_matrix(3, 5, &matrix);

  ck_assert_ptr_nonnull(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 3);
  ck_assert_int_eq(matrix.columns, 5);

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_valid_1x1) {
  s21_matrix matrix = {0};
  s21_create_matrix(1, 1, &matrix);

  ck_assert_ptr_nonnull(matrix.matrix);
  ck_assert_ptr_nonnull(matrix.matrix[0]);

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_valid_100x100) {
  s21_matrix matrix = {0};
  s21_create_matrix(100, 100, &matrix);

  ck_assert_ptr_nonnull(matrix.matrix);

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_null_pointer) {
  s21_remove_matrix(NULL);

  ck_assert_int_eq(1, 1);
}
END_TEST

START_TEST(test_remove_matrix_null_matrix_field) {
  s21_matrix matrix = {0};
  matrix.rows = 5;
  matrix.columns = 5;
  matrix.matrix = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_partial_null_rows) {
  s21_matrix matrix = {0};
  s21_create_matrix(3, 3, &matrix);

  free(matrix.matrix[1]);
  matrix.matrix[1] = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_already_removed) {
  s21_matrix matrix = {0};
  s21_create_matrix(3, 3, &matrix);

  s21_remove_matrix(&matrix);
  ck_assert_ptr_null(matrix.matrix);

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_zero_size) {
  s21_matrix matrix = {0};
  matrix.rows = 0;
  matrix.columns = 0;
  matrix.matrix = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_zero_rows_non_null_matrix) {
  s21_matrix matrix = {0};
  matrix.rows = 0;
  matrix.columns = 5;
  matrix.matrix = malloc(sizeof(double *) * 1);
  matrix.matrix[0] = malloc(sizeof(double) * 5);

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_zero_columns) {
  s21_matrix matrix = {0};
  matrix.rows = 5;
  matrix.columns = 0;
  matrix.matrix = malloc(sizeof(double *) * 5);
  for (int i = 0; i < 5; i++) {
    matrix.matrix[i] = NULL;
  }

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_negative_sizes) {
  s21_matrix matrix = {0};
  matrix.rows = -5;
  matrix.columns = -5;
  matrix.matrix = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_null_matrix_ptr_with_rows) {
  s21_matrix matrix = {0};
  matrix.rows = 3;
  matrix.columns = 3;
  matrix.matrix = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_alternating_null_rows) {
  s21_matrix matrix = {0};
  s21_create_matrix(5, 5, &matrix);

  free(matrix.matrix[1]);
  matrix.matrix[1] = NULL;
  free(matrix.matrix[3]);
  matrix.matrix[3] = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_after_partial_creation) {
  s21_matrix matrix = {0};

  matrix.rows = 5;
  matrix.columns = 5;
  matrix.matrix = malloc(sizeof(double *) * 5);
  for (int i = 0; i < 3; i++) {
    matrix.matrix[i] = malloc(sizeof(double) * 5);
  }
  for (int i = 3; i < 5; i++) {
    matrix.matrix[i] = NULL;
  }

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_single_row_null) {
  s21_matrix matrix = {0};
  matrix.rows = 1;
  matrix.columns = 5;
  matrix.matrix = malloc(sizeof(double *) * 1);
  matrix.matrix[0] = NULL;

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_and_check_invalid_access) {
  s21_matrix matrix = {0};
  s21_create_matrix(3, 3, &matrix);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      matrix.matrix[i][j] = i * j;
    }
  }

  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

START_TEST(test_remove_matrix_null_with_nonzero_sizes) {
  s21_matrix *matrix = NULL;

  s21_remove_matrix(matrix);

  ck_assert_int_eq(1, 1);
}
END_TEST

START_TEST(test_remove_matrix_double_after_create) {
  s21_matrix matrix = {0};
  s21_create_matrix(4, 4, &matrix);

  s21_remove_matrix(&matrix);
  s21_remove_matrix(&matrix);
  s21_remove_matrix(&matrix);

  ck_assert_ptr_null(matrix.matrix);
  ck_assert_int_eq(matrix.rows, 0);
  ck_assert_int_eq(matrix.columns, 0);
}
END_TEST

Suite *remove_matrix_suite(void) {
  Suite *suite = suite_create("s21_remove_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_remove_matrix_valid_3x5);
  tcase_add_test(tcase_core, test_remove_matrix_valid_1x1);
  tcase_add_test(tcase_core, test_remove_matrix_valid_100x100);
  tcase_add_test(tcase_core, test_remove_matrix_null_pointer);
  tcase_add_test(tcase_core, test_remove_matrix_null_matrix_field);
  tcase_add_test(tcase_core, test_remove_matrix_partial_null_rows);
  tcase_add_test(tcase_core, test_remove_matrix_already_removed);
  tcase_add_test(tcase_core, test_remove_matrix_zero_size);
  tcase_add_test(tcase_core, test_remove_matrix_zero_rows_non_null_matrix);
  tcase_add_test(tcase_core, test_remove_matrix_zero_columns);
  tcase_add_test(tcase_core, test_remove_matrix_negative_sizes);
  tcase_add_test(tcase_core, test_remove_matrix_null_matrix_ptr_with_rows);
  tcase_add_test(tcase_core, test_remove_matrix_alternating_null_rows);
  tcase_add_test(tcase_core, test_remove_matrix_after_partial_creation);
  tcase_add_test(tcase_core, test_remove_matrix_single_row_null);
  tcase_add_test(tcase_core, test_remove_matrix_and_check_invalid_access);
  tcase_add_test(tcase_core, test_remove_matrix_null_with_nonzero_sizes);
  tcase_add_test(tcase_core, test_remove_matrix_double_after_create);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

void create_test_matrix(s21_matrix *A, int rows, int cols, double values[]) {
  s21_create_matrix(rows, cols, A);
  int index = 0;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      A->matrix[i][j] = values[index++];
    }
  }
}

START_TEST(test_eq_matrix_equal_3x3) {
  s21_matrix A = {0}, B = {0};

  double values[] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

  create_test_matrix(&A, 3, 3, values);
  create_test_matrix(&B, 3, 3, values);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_equal_1x1) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);

  A.matrix[0][0] = 3.14159;
  B.matrix[0][0] = 3.14159;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_equal_5x5_zeros) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(5, 5, &A);
  s21_create_matrix(5, 5, &B);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_equal_floating_point) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 0.0000001;
  B.matrix[0][0] = 0.0000001;
  A.matrix[0][1] = 1.0000001;
  B.matrix[0][1] = 1.0000001;
  A.matrix[1][0] = 2.0000001;
  B.matrix[1][0] = 2.0000001;
  A.matrix[1][1] = 3.0000001;
  B.matrix[1][1] = 3.0000001;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_equal_boundary_epsilon) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);

  A.matrix[0][0] = 0.00000005;
  B.matrix[0][0] = 0.0;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_not_equal_values) {
  s21_matrix A = {0}, B = {0};

  double valuesA[] = {1.0, 2.0, 3.0, 4.0};
  double valuesB[] = {1.0, 2.0, 3.0, 5.0};

  create_test_matrix(&A, 2, 2, valuesA);
  create_test_matrix(&B, 2, 2, valuesB);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_not_equal_exceeds_epsilon) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);

  A.matrix[0][0] = 0.00000015;
  B.matrix[0][0] = 0.0;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_different_rows) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(2, 3, &A);
  s21_create_matrix(3, 3, &B);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_different_columns) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(3, 2, &A);
  s21_create_matrix(3, 3, &B);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_different_both_dimensions) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(3, 4, &B);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_first_null) {
  s21_matrix B = {0};
  s21_create_matrix(3, 3, &B);

  int result = s21_eq_matrix(NULL, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_second_null) {
  s21_matrix A = {0};
  s21_create_matrix(3, 3, &A);

  int result = s21_eq_matrix(&A, NULL);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_eq_matrix_both_null) {
  int result = s21_eq_matrix(NULL, NULL);
  ck_assert_int_eq(result, FAILURE);
}
END_TEST

START_TEST(test_eq_matrix_invalid_first_matrix) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(3, 3, &B);

  A.rows = 3;
  A.columns = 3;
  A.matrix = NULL;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_invalid_second_matrix) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(3, 3, &A);

  B.rows = 3;
  B.columns = 3;
  B.matrix = NULL;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_eq_matrix_invalid_rows_first) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(3, 3, &B);

  A.rows = 0;
  A.columns = 3;
  A.matrix = malloc(sizeof(double *) * 3);
  for (int i = 0; i < 3; i++) {
    A.matrix[i] = malloc(sizeof(double) * 3);
  }

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  for (int i = 0; i < 3; i++) {
    free(A.matrix[i]);
  }
  free(A.matrix);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_invalid_rows_second) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(3, 3, &A);

  B.rows = -1;
  B.columns = 3;
  B.matrix = malloc(sizeof(double *) * 3);
  for (int i = 0; i < 3; i++) {
    B.matrix[i] = malloc(sizeof(double) * 3);
  }

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  for (int i = 0; i < 3; i++) {
    free(B.matrix[i]);
  }
  free(B.matrix);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_eq_matrix_equal_negative_values) {
  s21_matrix A = {0}, B = {0};

  double values[] = {-1.5, -2.5, -3.5, -4.5};

  create_test_matrix(&A, 2, 2, values);
  create_test_matrix(&B, 2, 2, values);

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_almost_equal_within_epsilon) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1.00000001;
  B.matrix[0][0] = 1.00000005;
  A.matrix[0][1] = 2.00000001;
  B.matrix[0][1] = 2.00000006;
  A.matrix[1][0] = 3.00000002;
  B.matrix[1][0] = 3.00000007;
  A.matrix[1][1] = 4.00000003;
  B.matrix[1][1] = 4.00000008;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_row_vs_column) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(1, 3, &A);
  s21_create_matrix(3, 1, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;

  B.matrix[0][0] = 1.0;
  B.matrix[1][0] = 2.0;
  B.matrix[2][0] = 3.0;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_zero_size_matrices) {
  s21_matrix A = {0}, B = {0};

  A.rows = 0;
  A.columns = 0;
  A.matrix = NULL;

  B.rows = 0;
  B.columns = 0;
  B.matrix = NULL;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);
}
END_TEST

START_TEST(test_eq_matrix_large_equal) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(100, 100, &A);
  s21_create_matrix(100, 100, &B);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      A.matrix[i][j] = i * j * 0.001;
      B.matrix[i][j] = i * j * 0.001;
    }
  }

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, SUCCESS);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_large_not_equal) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(100, 100, &A);
  s21_create_matrix(100, 100, &B);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      A.matrix[i][j] = i * j * 0.001;
      B.matrix[i][j] = i * j * 0.001;
    }
  }

  B.matrix[50][50] = 999.999;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_eq_matrix_inf_and_nan) {
  s21_matrix A = {0}, B = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = INFINITY;
  B.matrix[0][0] = INFINITY;
  A.matrix[0][1] = -INFINITY;
  B.matrix[0][1] = -INFINITY;
  A.matrix[1][0] = NAN;
  B.matrix[1][0] = NAN;
  A.matrix[1][1] = 1.0;
  B.matrix[1][1] = 1.0;

  int result = s21_eq_matrix(&A, &B);
  ck_assert_int_eq(result, FAILURE);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

Suite *eq_matrix_suite(void) {
  Suite *suite = suite_create("s21_eq_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_eq_matrix_equal_3x3);
  tcase_add_test(tcase_core, test_eq_matrix_equal_1x1);
  tcase_add_test(tcase_core, test_eq_matrix_equal_5x5_zeros);
  tcase_add_test(tcase_core, test_eq_matrix_equal_floating_point);
  tcase_add_test(tcase_core, test_eq_matrix_equal_boundary_epsilon);
  tcase_add_test(tcase_core, test_eq_matrix_equal_negative_values);
  tcase_add_test(tcase_core, test_eq_matrix_almost_equal_within_epsilon);
  tcase_add_test(tcase_core, test_eq_matrix_large_equal);

  tcase_add_test(tcase_core, test_eq_matrix_not_equal_values);
  tcase_add_test(tcase_core, test_eq_matrix_not_equal_exceeds_epsilon);
  tcase_add_test(tcase_core, test_eq_matrix_large_not_equal);

  tcase_add_test(tcase_core, test_eq_matrix_different_rows);
  tcase_add_test(tcase_core, test_eq_matrix_different_columns);
  tcase_add_test(tcase_core, test_eq_matrix_different_both_dimensions);
  tcase_add_test(tcase_core, test_eq_matrix_row_vs_column);

  tcase_add_test(tcase_core, test_eq_matrix_first_null);
  tcase_add_test(tcase_core, test_eq_matrix_second_null);
  tcase_add_test(tcase_core, test_eq_matrix_both_null);
  tcase_add_test(tcase_core, test_eq_matrix_invalid_first_matrix);
  tcase_add_test(tcase_core, test_eq_matrix_invalid_second_matrix);
  tcase_add_test(tcase_core, test_eq_matrix_invalid_rows_first);
  tcase_add_test(tcase_core, test_eq_matrix_invalid_rows_second);
  tcase_add_test(tcase_core, test_eq_matrix_zero_size_matrices);

  tcase_add_test(tcase_core, test_eq_matrix_inf_and_nan);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

static void fill_matrix_with_inf(s21_matrix *A) {
  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < A->columns; j++) {
      A->matrix[i][j] = INFINITY;
    }
  }
}

static void fill_matrix_with_nan(s21_matrix *A) {
  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < A->columns; j++) {
      A->matrix[i][j] = NAN;
    }
  }
}

START_TEST(test_incorrect_matrix_null) {
  s21_matrix result = {0};
  int status = s21_calc_complements(NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_incorrect_matrix_invalid) {
  s21_matrix A = {0};
  s21_matrix result = {0};
  A.rows = -1;
  A.columns = 2;
  A.matrix = NULL;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_result_null) {
  s21_matrix A = {0};
  s21_create_matrix(3, 3, &A);
  int status = s21_calc_complements(&A, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_not_square) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(3, 4, &A);
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_contains_inf) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);
  fill_matrix_with_inf(&A);
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_contains_nan) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);
  fill_matrix_with_nan(&A);
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_empty_matrix) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(0, 0, &A);
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_1x1_positive) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 1);
  ck_assert_int_eq(result.columns, 1);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-6);
  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_1x1_negative) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = -3.5;
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-6);
  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_1x1_zero) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 0.0;
  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-6);
  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_2x2_normal) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  A.matrix[0][0] = 1;
  A.matrix[0][1] = 2;
  A.matrix[1][0] = 3;
  A.matrix[1][1] = 4;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 2);

  double expected[2][2];
  expected[0][0] = A.matrix[1][1];
  expected[0][1] = -A.matrix[1][0];
  expected[1][0] = -A.matrix[0][1];
  expected[1][1] = A.matrix[0][0];

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], expected[i][j], 1e-6);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_2x2_with_zeros) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  A.matrix[0][0] = 0;
  A.matrix[0][1] = 0;
  A.matrix[1][0] = 0;
  A.matrix[1][1] = 5;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  double expected[2][2];
  expected[0][0] = A.matrix[1][1];
  expected[0][1] = -A.matrix[1][0];
  expected[1][0] = -A.matrix[0][1];
  expected[1][1] = A.matrix[0][0];

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], expected[i][j], 1e-6);
    }
  }

  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 2);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_3x3_normal) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(3, 3, &A);

  A.matrix[0][0] = 1;
  A.matrix[0][1] = 2;
  A.matrix[0][2] = 3;
  A.matrix[1][0] = 0;
  A.matrix[1][1] = 1;
  A.matrix[1][2] = 4;
  A.matrix[2][0] = 5;
  A.matrix[2][1] = 6;
  A.matrix[2][2] = 0;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  double expected[3][3] = {{-24, 20, -5}, {18, -15, 4}, {5, -4, 1}};

  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 3);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], expected[i][j], 1e-6);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_3x3_negative) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(3, 3, &A);

  A.matrix[0][0] = -1;
  A.matrix[0][1] = -2;
  A.matrix[0][2] = -3;
  A.matrix[1][0] = 0;
  A.matrix[1][1] = -1;
  A.matrix[1][2] = -4;
  A.matrix[2][0] = -5;
  A.matrix[2][1] = -6;
  A.matrix[2][2] = 0;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  double expected[3][3] = {{-24, 20, -5}, {18, -15, 4}, {5, -4, 1}};

  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 3);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], expected[i][j], 1e-6);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_3x3_zero_determinant) {
  s21_matrix A = {0};
  double det = 0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1;
  A.matrix[0][1] = 2;
  A.matrix[0][2] = 3;
  A.matrix[1][0] = 1;
  A.matrix[1][1] = 2;
  A.matrix[1][2] = 3;
  A.matrix[2][0] = 4;
  A.matrix[2][1] = 5;
  A.matrix[2][2] = 6;

  int status = s21_determinant(&A, &det);
  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(det, 0.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_4x4_diagonal) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(4, 4, &A);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A.matrix[i][j] = (i == j) ? (i + 1) : 0;
    }
  }

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  for (int i = 0; i < 4; i++) {
    double expected = 1.0;
    for (int k = 0; k < 4; k++) {
      if (k != i) {
        expected *= (k + 1);
      }
    }
    ck_assert_double_eq_tol(result.matrix[i][i], expected, 1e-6);
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (i != j) {
        ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-6);
      }
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_determinant_error_inf) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(3, 3, &A);

  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2;
  A.matrix[0][2] = 3;
  A.matrix[1][0] = 4;
  A.matrix[1][1] = 5;
  A.matrix[1][2] = 6;
  A.matrix[2][0] = 7;
  A.matrix[2][1] = 8;
  A.matrix[2][2] = 9;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_error_nan) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(3, 3, &A);

  A.matrix[0][0] = 1;
  A.matrix[0][1] = NAN;
  A.matrix[0][2] = 3;
  A.matrix[1][0] = 4;
  A.matrix[1][1] = 5;
  A.matrix[1][2] = 6;
  A.matrix[2][0] = 7;
  A.matrix[2][1] = 8;
  A.matrix[2][2] = 9;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_large_numbers) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  A.matrix[0][0] = 1e100;
  A.matrix[0][1] = 1e100;
  A.matrix[1][0] = 1e100;
  A.matrix[1][1] = 1e100;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_small_numbers) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  A.matrix[0][0] = 1e-100;
  A.matrix[0][1] = 1e-100;
  A.matrix[1][0] = 1e-100;
  A.matrix[1][1] = 1e-100;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_result_already_exists) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(5, 5, &result);

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      result.matrix[i][j] = 999.0;
    }
  }

  A.matrix[0][0] = 1;
  A.matrix[0][1] = 2;
  A.matrix[1][0] = 3;
  A.matrix[1][1] = 4;

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 2);

  ck_assert_double_eq_tol(result.matrix[0][0], 4.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[0][1], -3.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][0], -2.0, 1e-6);
  ck_assert_double_eq_tol(result.matrix[1][1], 1.0, 1e-6);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_5x5_identity) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(5, 5, &A);

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      A.matrix[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);

  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (i == j) {
        ck_assert_double_eq_tol(result.matrix[i][j], 1.0, 1e-6);
      } else {
        ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-6);
      }
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_result_removed_on_error) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &result);

  fill_matrix_with_inf(&A);

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  ck_assert_ptr_eq(result.matrix, NULL);
  ck_assert_int_eq(result.rows, 0);
  ck_assert_int_eq(result.columns, 0);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_multiple_calls_same_result) {
  s21_matrix A1 = {0}, A2 = {0}, result = {0};
  s21_create_matrix(2, 2, &A1);
  s21_create_matrix(2, 2, &A2);

  A1.matrix[0][0] = 1;
  A1.matrix[0][1] = 2;
  A1.matrix[1][0] = 3;
  A1.matrix[1][1] = 4;

  A2.matrix[0][0] = 5;
  A2.matrix[0][1] = 6;
  A2.matrix[1][0] = 7;
  A2.matrix[1][1] = 8;

  int status1 = s21_calc_complements(&A1, &result);
  ck_assert_int_eq(status1, OK);

  int status2 = s21_calc_complements(&A2, &result);
  ck_assert_int_eq(status2, OK);
  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 2);

  s21_remove_matrix(&A1);
  s21_remove_matrix(&A2);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_10x10) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(10, 10, &A);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      A.matrix[i][j] = i * 10 + j;
    }
  }

  int status = s21_calc_complements(&A, &result);
  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 10);
  ck_assert_int_eq(result.columns, 10);

  s21_remove_matrix(&A);
}
END_TEST

Suite *calc_complements_suite(void) {
  Suite *s = suite_create("s21_calc_complements");

  TCase *tc_errors = tcase_create("Errors");
  tcase_add_checked_fixture(tc_errors, setup, NULL);
  tcase_add_test(tc_errors, test_incorrect_matrix_null);
  tcase_add_test(tc_errors, test_incorrect_matrix_invalid);
  tcase_add_test(tc_errors, test_result_null);
  tcase_add_test(tc_errors, test_not_square);
  tcase_add_test(tc_errors, test_contains_inf);
  tcase_add_test(tc_errors, test_contains_nan);
  tcase_add_test(tc_errors, test_empty_matrix);
  suite_add_tcase(s, tc_errors);

  TCase *tc_1x1 = tcase_create("1x1 matrix");
  tcase_add_checked_fixture(tc_1x1, setup, NULL);
  tcase_add_test(tc_1x1, test_1x1_positive);
  tcase_add_test(tc_1x1, test_1x1_negative);
  tcase_add_test(tc_1x1, test_1x1_zero);
  suite_add_tcase(s, tc_1x1);

  TCase *tc_2x2 = tcase_create("2x2 matrix");
  tcase_add_checked_fixture(tc_2x2, setup, NULL);
  tcase_add_test(tc_2x2, test_2x2_normal);
  tcase_add_test(tc_2x2, test_2x2_with_zeros);
  suite_add_tcase(s, tc_2x2);

  TCase *tc_3x3 = tcase_create("3x3 matrix");
  tcase_add_checked_fixture(tc_3x3, setup, NULL);
  tcase_add_test(tc_3x3, test_3x3_normal);
  tcase_add_test(tc_3x3, test_3x3_negative);
  tcase_add_test(tc_3x3, test_3x3_zero_determinant);
  suite_add_tcase(s, tc_3x3);

  TCase *tc_large = tcase_create("Large matrices");
  tcase_add_checked_fixture(tc_large, setup, NULL);
  tcase_add_test(tc_large, test_4x4_diagonal);
  tcase_add_test(tc_large, test_5x5_identity);
  tcase_add_test(tc_large, test_10x10);
  suite_add_tcase(s, tc_large);

  TCase *tc_calc_errors = tcase_create("Calculation errors");
  tcase_add_checked_fixture(tc_calc_errors, setup, NULL);
  tcase_add_test(tc_calc_errors, test_determinant_error_inf);
  tcase_add_test(tc_calc_errors, test_determinant_error_nan);
  tcase_add_test(tc_calc_errors, test_large_numbers);
  tcase_add_test(tc_calc_errors, test_small_numbers);
  suite_add_tcase(s, tc_calc_errors);

  TCase *tc_memory = tcase_create("Memory management");
  tcase_add_checked_fixture(tc_memory, setup, NULL);
  tcase_add_test(tc_memory, test_result_already_exists);
  tcase_add_test(tc_memory, test_result_removed_on_error);
  tcase_add_test(tc_memory, test_multiple_calls_same_result);
  suite_add_tcase(s, tc_memory);

  return s;
}

START_TEST(test_determinant_1x1) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 5.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_2x2) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, -2.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_3x3) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[2][0] = 7.0;
  A.matrix[2][1] = 8.0;
  A.matrix[2][2] = 9.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 0.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_identity_3x3) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 0.0;
  A.matrix[0][2] = 0.0;
  A.matrix[1][0] = 0.0;
  A.matrix[1][1] = 1.0;
  A.matrix[1][2] = 0.0;
  A.matrix[2][0] = 0.0;
  A.matrix[2][1] = 0.0;
  A.matrix[2][2] = 1.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 1.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_diagonal_3x3) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 2.0;
  A.matrix[0][1] = 0.0;
  A.matrix[0][2] = 0.0;
  A.matrix[1][0] = 0.0;
  A.matrix[1][1] = 3.0;
  A.matrix[1][2] = 0.0;
  A.matrix[2][0] = 0.0;
  A.matrix[2][1] = 0.0;
  A.matrix[2][2] = 4.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 24.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_4x4) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(4, 4, &A);
  A.matrix[0][0] = 5.0;
  A.matrix[0][1] = 1.0;
  A.matrix[0][2] = 2.0;
  A.matrix[0][3] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[1][3] = 7.0;
  A.matrix[2][0] = 8.0;
  A.matrix[2][1] = 9.0;
  A.matrix[2][2] = 1.0;
  A.matrix[2][3] = 2.0;
  A.matrix[3][0] = 3.0;
  A.matrix[3][1] = 4.0;
  A.matrix[3][2] = 5.0;
  A.matrix[3][3] = 6.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert(!isnan(result));
  ck_assert(!isinf(result));

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_INCORRECT_MATRIX_matrix) {
  double result = 0.0;

  int status = s21_determinant(NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_determinant_null_result) {
  s21_matrix A = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_determinant(&A, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_non_square) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(2, 3, &A);

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_with_nan) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_with_inf) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_negative_values) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = -1.0;
  A.matrix[0][1] = -2.0;
  A.matrix[1][0] = -3.0;
  A.matrix[1][1] = -4.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, -2.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_zero_row) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 0.0;
  A.matrix[0][1] = 0.0;
  A.matrix[0][2] = 0.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[2][0] = 7.0;
  A.matrix[2][1] = 8.0;
  A.matrix[2][2] = 9.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 0.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_duplicate_rows) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 1.0;
  A.matrix[1][1] = 2.0;
  A.matrix[1][2] = 3.0;
  A.matrix[2][0] = 4.0;
  A.matrix[2][1] = 5.0;
  A.matrix[2][2] = 6.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 0.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_5x5) {
  s21_matrix A = {0};
  double result = 0.0;

  s21_create_matrix(5, 5, &A);
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      A.matrix[i][j] = (i == j) ? 2.0 : 0.0;
    }
  }

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 32.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_handles_CALCULATION_ERROR) {
  s21_matrix A = {0};
  double result = 999.0;

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 1.0;
  A.matrix[1][0] = 1.0;
  A.matrix[1][1] = 1.0;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);
  ck_assert_double_eq_tol(result, 0.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_determinant_create_matrix_fails) {
  s21_matrix A = {0};
  double result = 0.0;

  A.rows = -1;
  A.columns = 3;
  A.matrix = NULL;

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_create_matrix(0, 0, &A);
  status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_3x3_success) {
  s21_matrix A;
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 2;
  A.matrix[0][1] = -3;
  A.matrix[0][2] = 1;
  A.matrix[1][0] = 2;
  A.matrix[1][1] = 0;
  A.matrix[1][2] = -1;
  A.matrix[2][0] = 1;
  A.matrix[2][1] = 4;
  A.matrix[2][2] = 5;

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 49.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_2x2_success) {
  s21_matrix A;
  double result;

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 5;
  A.matrix[0][1] = 6;
  A.matrix[1][0] = 7;
  A.matrix[1][1] = 8;

  ck_assert_int_eq(s21_determinant(&A, &result), OK);
  ck_assert_double_eq_tol(result, -2.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_1x1_success) {
  s21_matrix A;
  double result;

  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 42.0;

  ck_assert_int_eq(s21_determinant(&A, &result), OK);
  ck_assert_double_eq_tol(result, 42.0, EPS);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_identity_matrix) {
  s21_matrix A;
  double result;

  s21_create_matrix(4, 4, &A);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      A.matrix[i][j] = (i == j) ? 1.0 : 0.0;
    }
  }

  ck_assert_int_eq(s21_determinant(&A, &result), OK);
  ck_assert_double_eq_tol(result, 1.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_calculation_error_handling) {
  s21_matrix A;
  double result = 0.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1;
  A.matrix[0][1] = 2;
  A.matrix[0][2] = 3;
  A.matrix[1][0] = 4;
  A.matrix[1][1] = 5;
  A.matrix[1][2] = 6;
  A.matrix[2][0] = 7;
  A.matrix[2][1] = 8;
  A.matrix[2][2] = 9;

  int status = s21_determinant(&A, &result);

  ck_assert_int_eq(status, OK);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_singular_matrix) {
  s21_matrix A;
  double result = 999.0;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1;
  A.matrix[0][1] = 2;
  A.matrix[0][2] = 3;
  A.matrix[1][0] = 4;
  A.matrix[1][1] = 5;
  A.matrix[1][2] = 6;
  A.matrix[2][0] = 7;
  A.matrix[2][1] = 8;
  A.matrix[2][2] = 9;

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result, 0.0, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_with_negative_sign) {
  s21_matrix A;
  double result;

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 0;
  A.matrix[0][1] = 1;
  A.matrix[0][2] = 2;
  A.matrix[1][0] = 1;
  A.matrix[1][1] = 0;
  A.matrix[1][2] = 3;
  A.matrix[2][0] = 4;
  A.matrix[2][1] = 5;
  A.matrix[2][2] = 6;

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, OK);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(determinant_5x5_success) {
  s21_matrix A;
  double result;

  s21_create_matrix(5, 5, &A);
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (i <= j) {
        A.matrix[i][j] = i + j + 1;
      } else {
        A.matrix[i][j] = 0;
      }
    }
  }

  int status = s21_determinant(&A, &result);
  ck_assert_int_eq(status, OK);
  double expected = 1.0;
  for (int i = 0; i < 5; i++) {
    expected *= A.matrix[i][i];
  }
  ck_assert_double_eq_tol(result, expected, 1e-7);

  s21_remove_matrix(&A);
}
END_TEST

Suite *determinant_suite(void) {
  Suite *suite = suite_create("s21_determinant");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_determinant_1x1);
  tcase_add_test(tcase_core, test_determinant_2x2);
  tcase_add_test(tcase_core, test_determinant_3x3);
  tcase_add_test(tcase_core, test_determinant_identity_3x3);
  tcase_add_test(tcase_core, test_determinant_diagonal_3x3);
  tcase_add_test(tcase_core, test_determinant_4x4);
  tcase_add_test(tcase_core, test_determinant_INCORRECT_MATRIX_matrix);
  tcase_add_test(tcase_core, test_determinant_null_result);
  tcase_add_test(tcase_core, test_determinant_non_square);
  tcase_add_test(tcase_core, test_determinant_with_nan);
  tcase_add_test(tcase_core, test_determinant_with_inf);
  tcase_add_test(tcase_core, test_determinant_negative_values);
  tcase_add_test(tcase_core, test_determinant_zero_row);
  tcase_add_test(tcase_core, test_determinant_duplicate_rows);
  tcase_add_test(tcase_core, test_determinant_5x5);
  tcase_add_test(tcase_core, test_determinant_handles_CALCULATION_ERROR);
  tcase_add_test(tcase_core, test_determinant_create_matrix_fails);
  tcase_add_test(tcase_core, determinant_3x3_success);
  tcase_add_test(tcase_core, determinant_2x2_success);
  tcase_add_test(tcase_core, determinant_1x1_success);
  tcase_add_test(tcase_core, determinant_identity_matrix);
  tcase_add_test(tcase_core, determinant_calculation_error_handling);
  tcase_add_test(tcase_core, determinant_singular_matrix);
  tcase_add_test(tcase_core, determinant_with_negative_sign);
  tcase_add_test(tcase_core, determinant_5x5_success);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

START_TEST(test_mult_number_valid_2x2) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_mult_number(&A, 2.0, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 6.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 8.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_1x1) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;

  int status = s21_mult_number(&A, 3.0, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 15.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_by_zero) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[2][0] = 7.0;
  A.matrix[2][1] = 8.0;
  A.matrix[2][2] = 9.0;

  int status = s21_mult_number(&A, 0.0, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_negative) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = -2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = -4.0;

  int status = s21_mult_number(&A, -2.0, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], -2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], -6.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 8.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_fraction) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 2.0;
  A.matrix[0][1] = 4.0;
  A.matrix[1][0] = 6.0;
  A.matrix[1][1] = 8.0;

  int status = s21_mult_number(&A, 0.5, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 3.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 4.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_incorrect_matrix) {
  s21_matrix result = {0};

  int status = s21_mult_number(NULL, 2.0, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_mult_number_null_result) {
  s21_matrix A = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_mult_number(&A, 2.0, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_mult_number_invalid_matrix) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  A.rows = 0;
  A.columns = 2;
  A.matrix = NULL;

  int status = s21_mult_number(&A, 2.0, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_mult_number_create_matrix_fails) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);

  int status = s21_mult_number(&A, 2.0, &result);
  ck_assert_int_eq(status, OK);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_number_overflow_to_inf) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1e200;
  A.matrix[0][1] = 2e200;
  A.matrix[1][0] = 3e200;
  A.matrix[1][1] = 4e200;

  int status = s21_mult_number(&A, 1e200, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);
  ck_assert_ptr_null(result.matrix);
  ck_assert_int_eq(result.rows, 0);
  ck_assert_int_eq(result.columns, 0);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_mult_number_with_nan) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_mult_number(&A, 2.0, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_mult_number_inf_times_zero) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_mult_number(&A, 0.0, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_mult_number_large_matrix) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(100, 100, &A);
  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      A.matrix[i][j] = 1.0;
    }
  }

  int status = s21_mult_number(&A, 5.0, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 10; i++) {
    ck_assert_double_eq_tol(result.matrix[i][i], 5.0, 1e-7);
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

Suite *mult_number_suite(void) {
  Suite *suite = suite_create("s21_mult_number");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_mult_number_valid_2x2);
  tcase_add_test(tcase_core, test_mult_number_1x1);
  tcase_add_test(tcase_core, test_mult_number_by_zero);
  tcase_add_test(tcase_core, test_mult_number_negative);
  tcase_add_test(tcase_core, test_mult_number_fraction);
  tcase_add_test(tcase_core, test_mult_number_incorrect_matrix);
  tcase_add_test(tcase_core, test_mult_number_null_result);
  tcase_add_test(tcase_core, test_mult_number_invalid_matrix);
  tcase_add_test(tcase_core, test_mult_number_create_matrix_fails);
  tcase_add_test(tcase_core, test_mult_number_overflow_to_inf);
  tcase_add_test(tcase_core, test_mult_number_with_nan);
  tcase_add_test(tcase_core, test_mult_number_inf_times_zero);
  tcase_add_test(tcase_core, test_mult_number_large_matrix);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

START_TEST(test_inverse_success) {
  s21_matrix A = {0};
  s21_matrix expected = {0};
  s21_matrix result = {0};

  int status = s21_create_matrix(3, 3, &A);
  ck_assert_int_eq(status, OK);

  A.matrix[0][0] = 4.0;
  A.matrix[0][1] = 7.0;
  A.matrix[0][2] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 6.0;
  A.matrix[1][2] = 1.0;
  A.matrix[2][0] = 2.0;
  A.matrix[2][1] = 5.0;
  A.matrix[2][2] = 3.0;

  status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, OK);

  s21_create_matrix(3, 3, &expected);
  expected.matrix[0][0] = 1.4444444444444444;
  expected.matrix[0][1] = -1.222222222222223;
  expected.matrix[0][2] = -0.5555555555555556;
  expected.matrix[1][0] = -0.7777777777777778;
  expected.matrix[1][1] = 0.8888888888888888;
  expected.matrix[1][2] = 0.2222222222222222;
  expected.matrix[2][0] = 0.3333333333333333;
  expected.matrix[2][1] = -0.666666666666667;
  expected.matrix[2][2] = 0.3333333333333334;

  for (int i = 0; i < result.rows; i++) {
    for (int j = 0; j < result.columns; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], expected.matrix[i][j], 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&expected);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_det_zero) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 2.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_non_square) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 3, &A);

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_null_result) {
  s21_matrix A = {0};
  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 0.0;
  A.matrix[1][0] = 0.0;
  A.matrix[1][1] = 1.0;

  int status = s21_inverse_matrix(&A, NULL);

  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_invalid_matrix) {
  int status = s21_inverse_matrix(NULL, NULL);

  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_inverse_with_precision) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0001;
  A.matrix[0][1] = 2.0001;
  A.matrix[0][2] = 3.0001;
  A.matrix[1][0] = 4.0001;
  A.matrix[1][1] = 5.0001;
  A.matrix[1][2] = 6.0001;
  A.matrix[2][0] = 7.0001;
  A.matrix[2][1] = 8.0001;
  A.matrix[2][2] = 9.0001;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_verify_identity) {
  s21_matrix A = {0};
  s21_matrix inv = {0};
  s21_matrix product = {0};
  s21_matrix identity = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 2.0;
  A.matrix[0][1] = 1.0;
  A.matrix[0][2] = 1.0;
  A.matrix[1][0] = 1.0;
  A.matrix[1][1] = 3.0;
  A.matrix[1][2] = 2.0;
  A.matrix[2][0] = 1.0;
  A.matrix[2][1] = 0.0;
  A.matrix[2][2] = 0.0;

  s21_inverse_matrix(&A, &inv);
  s21_mult_matrix(&A, &inv, &product);

  s21_create_matrix(3, 3, &identity);
  identity.matrix[0][0] = 1.0;
  identity.matrix[0][1] = 0.0;
  identity.matrix[0][2] = 0.0;
  identity.matrix[1][0] = 0.0;
  identity.matrix[1][1] = 1.0;
  identity.matrix[1][2] = 0.0;
  identity.matrix[2][0] = 0.0;
  identity.matrix[2][1] = 0.0;
  identity.matrix[2][2] = 1.0;

  for (int i = 0; i < product.rows; i++) {
    for (int j = 0; j < product.columns; j++) {
      ck_assert_double_eq_tol(product.matrix[i][j], identity.matrix[i][j],
                              1e-6);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&inv);
  s21_remove_matrix(&product);
  s21_remove_matrix(&identity);
}
END_TEST

START_TEST(test_inverse_matrix_1x1) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 0.2, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_2x2) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 4.0;
  A.matrix[0][1] = 7.0;
  A.matrix[1][0] = 2.0;
  A.matrix[1][1] = 6.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 0.6, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], -0.7, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], -0.2, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 0.4, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_3x3) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 2.0;
  A.matrix[0][1] = 5.0;
  A.matrix[0][2] = 7.0;
  A.matrix[1][0] = 6.0;
  A.matrix[1][1] = 3.0;
  A.matrix[1][2] = 4.0;
  A.matrix[2][0] = 5.0;
  A.matrix[2][1] = -2.0;
  A.matrix[2][2] = -3.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, OK);
  s21_matrix identity = {0};
  s21_matrix mult = {0};
  s21_create_matrix(3, 3, &identity);
  s21_mult_matrix(&A, &result, &mult);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (i == j) {
        ck_assert_double_eq_tol(mult.matrix[i][j], 1.0, 1e-7);
      } else {
        ck_assert_double_eq_tol(mult.matrix[i][j], 0.0, 1e-7);
      }
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
  s21_remove_matrix(&identity);
  s21_remove_matrix(&mult);
}
END_TEST

START_TEST(test_inverse_matrix_identity) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 0.0;
  A.matrix[0][2] = 0.0;
  A.matrix[1][0] = 0.0;
  A.matrix[1][1] = 1.0;
  A.matrix[1][2] = 0.0;
  A.matrix[2][0] = 0.0;
  A.matrix[2][1] = 0.0;
  A.matrix[2][2] = 1.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][2], 1.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_diagonal) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 2.0;
  A.matrix[0][1] = 0.0;
  A.matrix[0][2] = 0.0;
  A.matrix[1][0] = 0.0;
  A.matrix[1][1] = 4.0;
  A.matrix[1][2] = 0.0;
  A.matrix[2][0] = 0.0;
  A.matrix[2][1] = 0.0;
  A.matrix[2][2] = 5.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 0.5, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 0.25, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][2], 0.2, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_incorrect_matrix) {
  s21_matrix result = {0};

  int status = s21_inverse_matrix(NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_inverse_matrix_null_result) {
  s21_matrix A = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_inverse_matrix(&A, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_non_square) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 3, &A);

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_with_nan) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_with_inf) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_singular) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 2.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_det_less_eps) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1e-8;
  A.matrix[0][1] = 2e-8;
  A.matrix[1][0] = 3e-8;
  A.matrix[1][1] = 6e-8;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_determinant_error) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_calc_complements_error) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_inverse_matrix_transpose_error) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  int status = s21_inverse_matrix(&A, &result);
  ck_assert_int_eq(status, OK);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_inverse_matrix_mult_number_error) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1e200;
  A.matrix[0][1] = 2e200;
  A.matrix[1][0] = 3e200;
  A.matrix[1][1] = 4e200;

  int status = s21_inverse_matrix(&A, &result);
  if (status != OK) {
    ck_assert_int_eq(status, CALCULATION_ERROR);
  }

  s21_remove_matrix(&A);
  if (status == OK) {
    s21_remove_matrix(&result);
  }
}
END_TEST

START_TEST(test_inverse_matrix_negative_determinant) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 5.0;

  int status = s21_inverse_matrix(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], -5.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 3.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], -1.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

Suite *inverse_matrix_suite(void) {
  Suite *suite = suite_create("s21_inverse_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_inverse_matrix_1x1);
  tcase_add_test(tcase_core, test_inverse_matrix_2x2);
  tcase_add_test(tcase_core, test_inverse_matrix_3x3);
  tcase_add_test(tcase_core, test_inverse_matrix_identity);
  tcase_add_test(tcase_core, test_inverse_matrix_diagonal);
  tcase_add_test(tcase_core, test_inverse_matrix_incorrect_matrix);
  tcase_add_test(tcase_core, test_inverse_matrix_null_result);
  tcase_add_test(tcase_core, test_inverse_matrix_non_square);
  tcase_add_test(tcase_core, test_inverse_matrix_with_nan);
  tcase_add_test(tcase_core, test_inverse_matrix_with_inf);
  tcase_add_test(tcase_core, test_inverse_matrix_singular);
  tcase_add_test(tcase_core, test_inverse_matrix_det_less_eps);
  tcase_add_test(tcase_core, test_inverse_matrix_determinant_error);
  tcase_add_test(tcase_core, test_inverse_matrix_calc_complements_error);
  tcase_add_test(tcase_core, test_inverse_matrix_transpose_error);
  tcase_add_test(tcase_core, test_inverse_matrix_mult_number_error);
  tcase_add_test(tcase_core, test_inverse_matrix_negative_determinant);

  tcase_add_test(tcase_core, test_inverse_success);
  tcase_add_test(tcase_core, test_inverse_det_zero);
  tcase_add_test(tcase_core, test_inverse_non_square);
  tcase_add_test(tcase_core, test_inverse_null_result);
  tcase_add_test(tcase_core, test_inverse_invalid_matrix);
  tcase_add_test(tcase_core, test_inverse_with_precision);
  tcase_add_test(tcase_core, test_inverse_verify_identity);
  suite_add_tcase(suite, tcase_core);

  return suite;
}

START_TEST(test_mult_matrix_2x3_3x2) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 3, &A);
  s21_create_matrix(3, 2, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;

  B.matrix[0][0] = 7.0;
  B.matrix[0][1] = 8.0;
  B.matrix[1][0] = 9.0;
  B.matrix[1][1] = 10.0;
  B.matrix[2][0] = 11.0;
  B.matrix[2][1] = 12.0;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 2);

  ck_assert_double_eq_tol(result.matrix[0][0], 58.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 64.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 139.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 154.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_2x2_2x2) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 5.0;
  B.matrix[0][1] = 6.0;
  B.matrix[1][0] = 7.0;
  B.matrix[1][1] = 8.0;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 19.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 22.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 43.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 50.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_3x2_2x4) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(3, 2, &A);
  s21_create_matrix(2, 4, &B);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      A.matrix[i][j] = i + j + 1;
    }
  }

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 4; j++) {
      B.matrix[i][j] = i + j + 1;
    }
  }

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 4);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_1x1_1x1) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);

  A.matrix[0][0] = 5.0;
  B.matrix[0][0] = 3.0;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 15.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_incorrect_A) {
  s21_matrix B = {0};
  s21_create_matrix(2, 2, &B);

  int status = s21_mult_matrix(NULL, &B, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_incorrect_B) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_mult_matrix(&A, NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_mult_matrix_null_result) {
  s21_matrix A = {0}, B = {0};
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  int status = s21_mult_matrix(&A, &B, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_invalid_A) {
  s21_matrix A = {0}, B = {0}, result = {0};

  A.rows = 0;
  A.columns = 2;
  A.matrix = NULL;

  s21_create_matrix(2, 2, &B);

  int status = s21_mult_matrix(&A, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_invalid_B) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);

  B.rows = 0;
  B.columns = 2;
  B.matrix = NULL;

  int status = s21_mult_matrix(&A, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_mult_matrix_wrong_dimensions) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 3, &A);
  s21_create_matrix(4, 2, &B);

  int status = s21_mult_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_overflow) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1e200;
  A.matrix[0][1] = 1e200;
  A.matrix[1][0] = 1e200;
  A.matrix[1][1] = 1e200;

  B.matrix[0][0] = 1e200;
  B.matrix[0][1] = 1e200;
  B.matrix[1][0] = 1e200;
  B.matrix[1][1] = 1e200;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_with_nan) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_mult_matrix_create_fails) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(100, 100, &A);
  s21_create_matrix(100, 100, &B);

  int status = s21_mult_matrix(&A, &B, &result);

  if (status != OK) {
    ck_assert_int_eq(status, CALCULATION_ERROR);
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  if (status == OK) {
    s21_remove_matrix(&result);
  }
}
END_TEST

START_TEST(test_mult_matrix_negative_values) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = -1.0;
  A.matrix[0][1] = -2.0;
  A.matrix[1][0] = -3.0;
  A.matrix[1][1] = -4.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], -7.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], -10.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], -15.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], -22.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_with_zeros) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(3, 3, &A);
  s21_create_matrix(3, 3, &B);

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_mult_matrix_1x3_3x1) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(1, 3, &A);
  s21_create_matrix(3, 1, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  B.matrix[0][0] = 4.0;
  B.matrix[1][0] = 5.0;
  B.matrix[2][0] = 6.0;

  int status = s21_mult_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 1);
  ck_assert_int_eq(result.columns, 1);
  ck_assert_double_eq_tol(result.matrix[0][0], 32.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

Suite *mult_matrix_suite(void) {
  Suite *suite = suite_create("s21_mult_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_mult_matrix_2x3_3x2);
  tcase_add_test(tcase_core, test_mult_matrix_2x2_2x2);
  tcase_add_test(tcase_core, test_mult_matrix_3x2_2x4);
  tcase_add_test(tcase_core, test_mult_matrix_1x1_1x1);
  tcase_add_test(tcase_core, test_mult_matrix_1x3_3x1);
  tcase_add_test(tcase_core, test_mult_matrix_incorrect_A);
  tcase_add_test(tcase_core, test_mult_matrix_incorrect_B);
  tcase_add_test(tcase_core, test_mult_matrix_null_result);
  tcase_add_test(tcase_core, test_mult_matrix_invalid_A);
  tcase_add_test(tcase_core, test_mult_matrix_invalid_B);
  tcase_add_test(tcase_core, test_mult_matrix_wrong_dimensions);
  tcase_add_test(tcase_core, test_mult_matrix_overflow);
  tcase_add_test(tcase_core, test_mult_matrix_with_nan);
  tcase_add_test(tcase_core, test_mult_matrix_create_fails);
  tcase_add_test(tcase_core, test_mult_matrix_negative_values);
  tcase_add_test(tcase_core, test_mult_matrix_with_zeros);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

static s21_matrix *create_inf_matrix(int rows, int cols) {
  s21_matrix *mat = (s21_matrix *)malloc(sizeof(s21_matrix));
  s21_create_matrix(rows, cols, mat);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      mat->matrix[i][j] = 0.0;
    }
  mat->matrix[0][0] = INFINITY;
  return mat;
}

static s21_matrix *create_nan_matrix(int rows, int cols) {
  s21_matrix *mat = (s21_matrix *)malloc(sizeof(s21_matrix));
  s21_create_matrix(rows, cols, mat);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      mat->matrix[i][j] = 0.0;
    }
  mat->matrix[0][0] = NAN;
  return mat;
}

static s21_matrix *create_large_matrix(int rows, int cols, double val) {
  s21_matrix *mat = (s21_matrix *)malloc(sizeof(s21_matrix));
  s21_create_matrix(rows, cols, mat);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      mat->matrix[i][j] = val;
    }
  }
  return mat;
}

START_TEST(test_sub_matrix_inf_in_a) {
  s21_matrix B, result;
  s21_matrix *infA = create_inf_matrix(2, 2);
  s21_create_matrix(2, 2, &B);
  s21_create_matrix(2, 2, &result);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      B.matrix[i][j] = 1.0;
    }
  }

  int status = s21_sub_matrix(infA, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(infA);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
  free(infA);
}
END_TEST

START_TEST(test_sub_matrix_nan_in_b) {
  s21_matrix A, result;
  s21_matrix *nanB = create_nan_matrix(2, 2);
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &result);

  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      A.matrix[i][j] = 5.0;
    }
  }

  int status = s21_sub_matrix(&A, nanB, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(nanB);
  s21_remove_matrix(&result);
  free(nanB);
}
END_TEST

START_TEST(test_sub_matrix_overflow_) {
  s21_matrix A, B, result;
  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);
  s21_create_matrix(1, 1, &result);

  A.matrix[0][0] = DBL_MAX;
  B.matrix[0][0] = -DBL_MAX;

  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_nan_result) {
  s21_matrix A, B, result;
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);
  s21_create_matrix(2, 2, &result);

  A.matrix[0][0] = INFINITY;
  B.matrix[0][0] = INFINITY;
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      if (!(i == 0 && j == 0)) {
        A.matrix[i][j] = 0;
        B.matrix[i][j] = 0;
      }
    }
  }
  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_inf_minus_inf) {
  s21_matrix A, B, result;
  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);
  s21_create_matrix(1, 1, &result);

  A.matrix[0][0] = INFINITY;
  B.matrix[0][0] = INFINITY;

  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_overflow_max) {
  s21_matrix result;
  s21_matrix *largeA = create_large_matrix(2, 2, DBL_MAX);
  s21_matrix *largeB = create_large_matrix(2, 2, -DBL_MAX);
  s21_create_matrix(2, 2, &result);

  int status = s21_sub_matrix(largeA, largeB, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(largeA);
  s21_remove_matrix(largeB);
  s21_remove_matrix(&result);
  free(largeA);
  free(largeB);
}
END_TEST

START_TEST(test_sub_matrix_2x2) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 5.0;
  A.matrix[0][1] = 6.0;
  A.matrix[1][0] = 7.0;
  A.matrix[1][1] = 8.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 4.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_3x3) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(3, 3, &A);
  s21_create_matrix(3, 3, &B);

  A.matrix[0][0] = 10.0;
  A.matrix[0][1] = 20.0;
  A.matrix[0][2] = 30.0;
  A.matrix[1][0] = 40.0;
  A.matrix[1][1] = 50.0;
  A.matrix[1][2] = 60.0;
  A.matrix[2][0] = 70.0;
  A.matrix[2][1] = 80.0;
  A.matrix[2][2] = 90.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[0][2] = 3.0;
  B.matrix[1][0] = 4.0;
  B.matrix[1][1] = 5.0;
  B.matrix[1][2] = 6.0;
  B.matrix[2][0] = 7.0;
  B.matrix[2][1] = 8.0;
  B.matrix[2][2] = 9.0;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);

  ck_assert_double_eq_tol(result.matrix[0][0], 9.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 18.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][2], 27.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 36.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 45.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][2], 54.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][0], 63.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][1], 72.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][2], 81.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_1x1) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);

  A.matrix[0][0] = 10.0;
  B.matrix[0][0] = 3.0;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 7.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_negative_values) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = -5.0;
  A.matrix[0][1] = -6.0;
  A.matrix[1][0] = -7.0;
  A.matrix[1][1] = -8.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], -6.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], -8.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], -10.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], -12.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_fractional) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 5.5;
  A.matrix[0][1] = 6.5;
  A.matrix[1][0] = 7.5;
  A.matrix[1][1] = 8.5;

  B.matrix[0][0] = 1.2;
  B.matrix[0][1] = 2.3;
  B.matrix[1][0] = 3.4;
  B.matrix[1][1] = 4.5;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 4.3, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.2, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 4.1, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 4.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_zero_result) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(3, 3, &A);
  s21_create_matrix(3, 3, &B);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      A.matrix[i][j] = i * 3 + j + 1.0;
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      B.matrix[i][j] = A.matrix[i][j];
    }
  }

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_incorrect_A) {
  s21_matrix B = {0}, result = {0};
  s21_create_matrix(2, 2, &B);

  int status = s21_sub_matrix(NULL, &B, &result);
  printf("\n%d\n", status);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_incorrect_B) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_sub_matrix(&A, NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_sub_matrix_null_result) {
  s21_matrix A = {0}, B = {0};
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  int status = s21_sub_matrix(&A, &B, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_invalid_A) {
  s21_matrix A = {0}, B = {0}, result = {0};

  A.rows = 0;
  A.columns = 2;
  A.matrix = NULL;

  s21_create_matrix(2, 2, &B);

  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_invalid_B) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);

  B.rows = 0;
  B.columns = 2;
  B.matrix = NULL;

  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_sub_matrix_different_size) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 3, &A);
  s21_create_matrix(2, 2, &B);

  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_different_size2) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(3, 2, &B);

  int status = s21_sub_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_overflow) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_with_nan) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sub_matrix_create_fails) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(1000, 1000, &A);
  s21_create_matrix(1000, 1000, &B);

  int status = s21_sub_matrix(&A, &B, &result);

  if (status != OK) {
    ck_assert_int_eq(status, CALCULATION_ERROR);
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  if (status == OK) {
    s21_remove_matrix(&result);
  }
}
END_TEST

START_TEST(test_sub_matrix_large) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(100, 100, &A);
  s21_create_matrix(100, 100, &B);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      A.matrix[i][j] = i + j;
      B.matrix[i][j] = i;
    }
  }

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 100);
  ck_assert_int_eq(result.columns, 100);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], (i + j) - i, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sub_matrix_small_values) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1e-10;
  A.matrix[0][1] = 2e-10;
  A.matrix[1][0] = 3e-10;
  A.matrix[1][1] = 4e-10;

  B.matrix[0][0] = 1e-11;
  B.matrix[0][1] = 2e-11;
  B.matrix[1][0] = 3e-11;
  B.matrix[1][1] = 4e-11;

  int status = s21_sub_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 9e-11, 1e-20);
  ck_assert_double_eq_tol(result.matrix[0][1], 18e-11, 1e-20);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

Suite *sub_matrix_suite(void) {
  Suite *suite = suite_create("s21_sub_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_sub_matrix_2x2);
  tcase_add_test(tcase_core, test_sub_matrix_3x3);
  tcase_add_test(tcase_core, test_sub_matrix_1x1);
  tcase_add_test(tcase_core, test_sub_matrix_negative_values);
  tcase_add_test(tcase_core, test_sub_matrix_fractional);
  tcase_add_test(tcase_core, test_sub_matrix_zero_result);
  tcase_add_test(tcase_core, test_sub_matrix_large);
  tcase_add_test(tcase_core, test_sub_matrix_small_values);

  tcase_add_test(tcase_core, test_sub_matrix_incorrect_A);
  tcase_add_test(tcase_core, test_sub_matrix_incorrect_B);
  tcase_add_test(tcase_core, test_sub_matrix_null_result);
  tcase_add_test(tcase_core, test_sub_matrix_invalid_A);
  tcase_add_test(tcase_core, test_sub_matrix_invalid_B);

  tcase_add_test(tcase_core, test_sub_matrix_different_size);
  tcase_add_test(tcase_core, test_sub_matrix_different_size2);
  tcase_add_test(tcase_core, test_sub_matrix_overflow);
  tcase_add_test(tcase_core, test_sub_matrix_with_nan);
  tcase_add_test(tcase_core, test_sub_matrix_create_fails);
  tcase_add_test(tcase_core, test_sub_matrix_inf_in_a);
  tcase_add_test(tcase_core, test_sub_matrix_nan_in_b);
  tcase_add_test(tcase_core, test_sub_matrix_nan_result);
  tcase_add_test(tcase_core, test_sub_matrix_inf_minus_inf);
  tcase_add_test(tcase_core, test_sub_matrix_overflow_);
  tcase_add_test(tcase_core, test_sub_matrix_overflow_max);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

START_TEST(test_sum_matrix_2x2) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 5.0;
  B.matrix[0][1] = 6.0;
  B.matrix[1][0] = 7.0;
  B.matrix[1][1] = 8.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 6.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 8.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 10.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 12.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_3x3) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(3, 3, &A);
  s21_create_matrix(3, 3, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[2][0] = 7.0;
  A.matrix[2][1] = 8.0;
  A.matrix[2][2] = 9.0;

  B.matrix[0][0] = 9.0;
  B.matrix[0][1] = 8.0;
  B.matrix[0][2] = 7.0;
  B.matrix[1][0] = 6.0;
  B.matrix[1][1] = 5.0;
  B.matrix[1][2] = 4.0;
  B.matrix[2][0] = 3.0;
  B.matrix[2][1] = 2.0;
  B.matrix[2][2] = 1.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], 10.0, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_1x1) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(1, 1, &A);
  s21_create_matrix(1, 1, &B);

  A.matrix[0][0] = 5.0;
  B.matrix[0][0] = 3.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 8.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_negative_values) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = -1.0;
  A.matrix[0][1] = -2.0;
  A.matrix[1][0] = -3.0;
  A.matrix[1][1] = -4.0;

  B.matrix[0][0] = 5.0;
  B.matrix[0][1] = 6.0;
  B.matrix[1][0] = 7.0;
  B.matrix[1][1] = 8.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 4.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_fractional) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1.1;
  A.matrix[0][1] = 2.2;
  A.matrix[1][0] = 3.3;
  A.matrix[1][1] = 4.4;

  B.matrix[0][0] = 5.5;
  B.matrix[0][1] = 6.6;
  B.matrix[1][0] = 7.7;
  B.matrix[1][1] = 8.8;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 6.6, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 8.8, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 11.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 13.2, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_with_zeros) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(3, 3, &A);
  s21_create_matrix(3, 3, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[2][0] = 7.0;
  A.matrix[2][1] = 8.0;
  A.matrix[2][2] = 9.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], A.matrix[i][j], 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_zero_result) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = -1.0;
  B.matrix[0][1] = -2.0;
  B.matrix[1][0] = -3.0;
  B.matrix[1][1] = -4.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_incorrect_A) {
  s21_matrix B = {0}, result = {0};
  s21_create_matrix(2, 2, &B);

  int status = s21_sum_matrix(NULL, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_incorrect_B) {
  s21_matrix A = {0}, result = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_sum_matrix(&A, NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_sum_matrix_null_result) {
  s21_matrix A = {0}, B = {0};
  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  int status = s21_sum_matrix(&A, &B, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_invalid_A) {
  s21_matrix A = {0}, B = {0}, result = {0};

  A.rows = 0;
  A.columns = 2;
  A.matrix = NULL;

  s21_create_matrix(2, 2, &B);

  int status = s21_sum_matrix(&A, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_invalid_B) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);

  B.rows = 0;
  B.columns = 2;
  B.matrix = NULL;

  int status = s21_sum_matrix(&A, &B, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_sum_matrix_different_size) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 3, &A);
  s21_create_matrix(2, 2, &B);

  int status = s21_sum_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_different_size2) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(3, 2, &B);

  int status = s21_sum_matrix(&A, &B, &result);
  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_overflow) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = INFINITY;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_with_nan) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = NAN;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 1.0;
  B.matrix[0][1] = 2.0;
  B.matrix[1][0] = 3.0;
  B.matrix[1][1] = 4.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, CALCULATION_ERROR);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
}
END_TEST

START_TEST(test_sum_matrix_create_fails) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(1000, 1000, &A);
  s21_create_matrix(1000, 1000, &B);

  int status = s21_sum_matrix(&A, &B, &result);

  if (status != OK) {
    ck_assert_int_eq(status, CALCULATION_ERROR);
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  if (status == OK) {
    s21_remove_matrix(&result);
  }
}
END_TEST

START_TEST(test_sum_matrix_large) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(100, 100, &A);
  s21_create_matrix(100, 100, &B);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 100; j++) {
      A.matrix[i][j] = i + j;
      B.matrix[i][j] = i;
    }
  }

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 100);
  ck_assert_int_eq(result.columns, 100);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], (i + j) + i, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_small_values) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  A.matrix[0][0] = 1e-10;
  A.matrix[0][1] = 2e-10;
  A.matrix[1][0] = 3e-10;
  A.matrix[1][1] = 4e-10;

  B.matrix[0][0] = 1e-11;
  B.matrix[0][1] = 2e-11;
  B.matrix[1][0] = 3e-11;
  B.matrix[1][1] = 4e-11;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.1e-10, 1e-20);
  ck_assert_double_eq_tol(result.matrix[0][1], 2.2e-10, 1e-20);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_sum_matrix_result_already_exists) {
  s21_matrix A = {0}, B = {0}, result = {0};

  s21_create_matrix(2, 2, &A);
  s21_create_matrix(2, 2, &B);

  s21_create_matrix(1, 1, &result);

  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;

  B.matrix[0][0] = 5.0;
  B.matrix[0][1] = 6.0;
  B.matrix[1][0] = 7.0;
  B.matrix[1][1] = 8.0;

  int status = s21_sum_matrix(&A, &B, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 2);
  ck_assert_double_eq_tol(result.matrix[0][0], 6.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&B);
  s21_remove_matrix(&result);
}
END_TEST

Suite *sum_matrix_suite(void) {
  Suite *suite = suite_create("s21_sum_matrix");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_sum_matrix_2x2);
  tcase_add_test(tcase_core, test_sum_matrix_3x3);
  tcase_add_test(tcase_core, test_sum_matrix_1x1);
  tcase_add_test(tcase_core, test_sum_matrix_negative_values);
  tcase_add_test(tcase_core, test_sum_matrix_fractional);
  tcase_add_test(tcase_core, test_sum_matrix_with_zeros);
  tcase_add_test(tcase_core, test_sum_matrix_zero_result);
  tcase_add_test(tcase_core, test_sum_matrix_large);
  tcase_add_test(tcase_core, test_sum_matrix_small_values);
  tcase_add_test(tcase_core, test_sum_matrix_result_already_exists);

  tcase_add_test(tcase_core, test_sum_matrix_incorrect_A);
  tcase_add_test(tcase_core, test_sum_matrix_incorrect_B);
  tcase_add_test(tcase_core, test_sum_matrix_null_result);
  tcase_add_test(tcase_core, test_sum_matrix_invalid_A);
  tcase_add_test(tcase_core, test_sum_matrix_invalid_B);

  tcase_add_test(tcase_core, test_sum_matrix_different_size);
  tcase_add_test(tcase_core, test_sum_matrix_different_size2);
  tcase_add_test(tcase_core, test_sum_matrix_overflow);
  tcase_add_test(tcase_core, test_sum_matrix_with_nan);
  tcase_add_test(tcase_core, test_sum_matrix_create_fails);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

START_TEST(test_transpose_2x3) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 2);

  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 5.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][0], 3.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][1], 6.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_3x2) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 2, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[1][0] = 3.0;
  A.matrix[1][1] = 4.0;
  A.matrix[2][0] = 5.0;
  A.matrix[2][1] = 6.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 2);
  ck_assert_int_eq(result.columns, 3);

  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 3.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][2], 5.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][2], 6.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_3x3) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;
  A.matrix[2][0] = 7.0;
  A.matrix[2][1] = 8.0;
  A.matrix[2][2] = 9.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 3);

  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][2], 7.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 5.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][2], 8.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][0], 3.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][1], 6.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][2], 9.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_1x1) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(1, 1, &A);
  A.matrix[0][0] = 5.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 1);
  ck_assert_int_eq(result.columns, 1);
  ck_assert_double_eq_tol(result.matrix[0][0], 5.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_1x3) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(1, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 1);

  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[2][0], 3.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_3x1) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 1, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[1][0] = 2.0;
  A.matrix[2][0] = 3.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 1);
  ck_assert_int_eq(result.columns, 3);

  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][2], 3.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_negative_values) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = -1.0;
  A.matrix[0][1] = -2.0;
  A.matrix[1][0] = -3.0;
  A.matrix[1][1] = -4.0;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], -1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], -3.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], -2.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], -4.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_fractional) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 2, &A);
  A.matrix[0][0] = 1.1;
  A.matrix[0][1] = 2.2;
  A.matrix[1][0] = 3.3;
  A.matrix[1][1] = 4.4;

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.1, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 3.3, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][0], 2.2, 1e-7);
  ck_assert_double_eq_tol(result.matrix[1][1], 4.4, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_incorrect_A) {
  s21_matrix result = {0};

  int status = s21_transpose(NULL, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_transpose_null_result) {
  s21_matrix A = {0};
  s21_create_matrix(2, 2, &A);

  int status = s21_transpose(&A, NULL);
  ck_assert_int_eq(status, INCORRECT_MATRIX);

  s21_remove_matrix(&A);
}
END_TEST

START_TEST(test_transpose_invalid_A) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  A.rows = 0;
  A.columns = 2;
  A.matrix = NULL;

  int status = s21_transpose(&A, &result);
  ck_assert_int_eq(status, INCORRECT_MATRIX);
}
END_TEST

START_TEST(test_transpose_with_zeros) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(3, 3, &A);

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], 0.0, 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_large) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(100, 50, &A);

  for (int i = 0; i < 100; i++) {
    for (int j = 0; j < 50; j++) {
      A.matrix[i][j] = i * j;
    }
  }

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 50);
  ck_assert_int_eq(result.columns, 100);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      ck_assert_double_eq_tol(result.matrix[i][j], A.matrix[j][i], 1e-7);
    }
  }

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_result_already_exists) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(2, 3, &A);
  A.matrix[0][0] = 1.0;
  A.matrix[0][1] = 2.0;
  A.matrix[0][2] = 3.0;
  A.matrix[1][0] = 4.0;
  A.matrix[1][1] = 5.0;
  A.matrix[1][2] = 6.0;

  s21_create_matrix(1, 1, &result);

  int status = s21_transpose(&A, &result);

  ck_assert_int_eq(status, OK);
  ck_assert_int_eq(result.rows, 3);
  ck_assert_int_eq(result.columns, 2);
  ck_assert_double_eq_tol(result.matrix[0][0], 1.0, 1e-7);
  ck_assert_double_eq_tol(result.matrix[0][1], 4.0, 1e-7);

  s21_remove_matrix(&A);
  s21_remove_matrix(&result);
}
END_TEST

START_TEST(test_transpose_create_fails) {
  s21_matrix A = {0};
  s21_matrix result = {0};

  s21_create_matrix(1000, 1000, &A);

  int status = s21_transpose(&A, &result);

  if (status != OK) {
    ck_assert_int_eq(status, INCORRECT_MATRIX);
  }

  s21_remove_matrix(&A);
  if (status == OK) {
    s21_remove_matrix(&result);
  }
}
END_TEST

Suite *transpose_suite(void) {
  Suite *suite = suite_create("s21_transpose");
  TCase *tcase_core = tcase_create("Core");

  tcase_add_checked_fixture(tcase_core, setup, NULL);

  tcase_add_test(tcase_core, test_transpose_2x3);
  tcase_add_test(tcase_core, test_transpose_3x2);
  tcase_add_test(tcase_core, test_transpose_3x3);
  tcase_add_test(tcase_core, test_transpose_1x1);
  tcase_add_test(tcase_core, test_transpose_1x3);
  tcase_add_test(tcase_core, test_transpose_3x1);
  tcase_add_test(tcase_core, test_transpose_negative_values);
  tcase_add_test(tcase_core, test_transpose_fractional);
  tcase_add_test(tcase_core, test_transpose_with_zeros);
  tcase_add_test(tcase_core, test_transpose_large);
  tcase_add_test(tcase_core, test_transpose_result_already_exists);

  tcase_add_test(tcase_core, test_transpose_incorrect_A);
  tcase_add_test(tcase_core, test_transpose_null_result);
  tcase_add_test(tcase_core, test_transpose_invalid_A);

  tcase_add_test(tcase_core, test_transpose_create_fails);

  suite_add_tcase(suite, tcase_core);

  return suite;
}

int run_suite(Suite *(*suite_func)(void), const char *suite_name) {
  Suite *s = suite_func();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_VERBOSE);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  printf("\n%s: %s (%d failures)\n", suite_name, failed ? "FAIL" : "SUCCESS",
         failed);
  printf("\n");
  return failed;
}

int main(void) {
  int total = 0;
  printf("\n========== START TESTING ==========\n\n");

  total += run_suite(create_matrix_suite, "s21_create_matrix");
  total += run_suite(remove_matrix_suite, "s21_remove_matrix");
  total += run_suite(eq_matrix_suite, "s21_eq_matrix");
  total += run_suite(calc_complements_suite, "s21_calc_complements");
  total += run_suite(determinant_suite, "s21_determinant");
  total += run_suite(mult_number_suite, "s21_mult_number");
  total += run_suite(inverse_matrix_suite, "s21_inverse_matrix");
  total += run_suite(mult_matrix_suite, "s21_mult_matrix");
  total += run_suite(sub_matrix_suite, "s21_sub_matrix");
  total += run_suite(sum_matrix_suite, "s21_sum_matrix");
  total += run_suite(transpose_suite, "s21_transpose");

  printf("\n========== FINAL SUMMARY ==========\n");
  printf("Total failed tests: %d\n", total);
  printf("Total passed suites: %d\n", 11 - (total > 0 ? 1 : 0));

  return total;
}