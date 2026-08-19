#include "s21_matrix.h"

int s21_mult_number(s21_matrix *A, double number, s21_matrix *result) {
  if (s21_check_matrix(A) != OK || result == NULL) {
    return INCORRECT_MATRIX;
  }
  if (result->matrix != NULL) {
    s21_remove_matrix(result);
  }

  int status = s21_create_matrix(A->rows, A->columns, result);

  for (int i = 0; i < A->rows && status == OK; i++) {
    for (int j = 0; j < A->columns; j++) {
      result->matrix[i][j] = A->matrix[i][j] * number;
      if (!isfinite(result->matrix[i][j])) {
        status = CALCULATION_ERROR;
      }
    }
  }
  if (status != OK) {
    s21_remove_matrix(result);
  }

  return status;
}