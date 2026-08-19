#include "s21_matrix.h"

int s21_transpose(s21_matrix *A, s21_matrix *result) {
  if (s21_check_matrix(A) != OK || result == NULL) {
    return INCORRECT_MATRIX;
  }
  if (result->matrix != NULL) {
    s21_remove_matrix(result);
  }

  int status = s21_create_matrix(A->columns, A->rows, result);

  for (int i = 0; i < A->rows && status == OK; i++) {
    for (int j = 0; j < A->columns; j++) {
      result->matrix[j][i] = A->matrix[i][j];
    }
  }
  return status;
}