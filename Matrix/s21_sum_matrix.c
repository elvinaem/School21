#include "s21_matrix.h"

int s21_sum_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result) {
  int status = OK;
  if (s21_check_matrix(A) != OK || s21_check_matrix(B) != OK ||
      result == NULL) {
    status = INCORRECT_MATRIX;
  } else if (!s21_eq_matrix_size(A, B) || s21_inf_or_nan(A) ||
             s21_inf_or_nan(B)) {
    status = CALCULATION_ERROR;
  } else {
    if (result->matrix != NULL) {
      s21_remove_matrix(result);
    }
    status = s21_create_matrix(A->rows, A->columns, result);

    for (int i = 0; i < A->rows && status == OK; i++) {
      for (int j = 0; j < A->columns; j++) {
        result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
        if (!isfinite(result->matrix[i][j])) {
          status = CALCULATION_ERROR;
        }
      }
    }
  }
  if (status != OK) {
    s21_remove_matrix(result);
  }
  return status;
}