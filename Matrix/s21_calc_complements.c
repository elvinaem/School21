#include "s21_matrix.h"

int s21_calc_complements(s21_matrix *A, s21_matrix *result) {
  int status = OK;
  if (s21_check_matrix(A) != OK || result == NULL) {
    status = INCORRECT_MATRIX;
  } else if (s21_is_square(A) != OK || s21_inf_or_nan(A)) {
    status = CALCULATION_ERROR;
  } else {
    if (result->matrix != NULL) {
      s21_remove_matrix(result);
    }
    status = s21_create_matrix(A->rows, A->columns, result);
    s21_matrix minor = {0};
    int flag = 0;

    if (status == OK) {
      if (A->rows > 1) {
        status = s21_create_matrix(A->rows - 1, A->columns - 1, &minor);
      } else {
        flag = 1;
        result->matrix[0][0] = 1.0;
      }
    }
    for (int i = 0; i < A->rows && status == OK && flag == 0; i++) {
      for (int j = 0; j < A->columns && status == OK; j++) {
        s21_minors_matrix(&minor, A, i, j);
        status = s21_determinant(&minor, &result->matrix[i][j]);
      }
    }
    if (status == OK && flag == 0) {
      s21_algebraic_complements(A, result);
    }

    s21_remove_matrix(&minor);
  }

  if (status != OK) {
    s21_remove_matrix(result);
  }
  return status;
}