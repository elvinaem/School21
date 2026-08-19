#include "s21_matrix.h"

int s21_eq_matrix(s21_matrix *A, s21_matrix *B) {
  int result = SUCCESS;

  if (s21_check_matrix(A) != OK || s21_check_matrix(B) != OK ||
      !s21_eq_matrix_size(A, B) || s21_inf_or_nan(A) == CALCULATION_ERROR ||
      s21_inf_or_nan(B) == CALCULATION_ERROR) {
    return FAILURE;
  }

  for (int i = 0; i < A->rows && result == SUCCESS; i++) {
    for (int j = 0; j < A->columns; j++) {
      if (fabs(A->matrix[i][j] - B->matrix[i][j]) > EPS) {
        result = FAILURE;
      }
    }
  }
  return result;
}