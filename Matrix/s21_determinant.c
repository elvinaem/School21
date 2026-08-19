#include "s21_matrix.h"

int s21_determinant(s21_matrix *A, double *result) {
  int status = OK;
  if (s21_check_matrix(A) != OK || result == NULL) {
    status = INCORRECT_MATRIX;
  } else if (s21_is_square(A) != OK || s21_inf_or_nan(A)) {
    status = CALCULATION_ERROR;
    if (s21_inf_or_nan(A) == CALCULATION_ERROR) {
      *result = 0.0;
    }
  } else {
    int sign = 1, det_fl = 0;
    s21_matrix copy = {0};
    status = s21_create_matrix(A->rows, A->columns, &copy);

    if (status == OK && det_fl == 0) {
      status = s21_copy_matrix(A, &copy);
    }
    if (status == OK && det_fl == 0) {
      status = s21_triangular_form(&copy, &sign);
    }
    if (status == OK && det_fl == 0) {
      *result = 1.0;
      for (int l = 0; l < copy.rows; l++) {
        *result *= copy.matrix[l][l];
      }
      *result *= sign;

    } else if (status == CALCULATION_ERROR && det_fl == 0) {
      *result = 0.0;
      status = OK;
    }
    if (copy.matrix != NULL) {
      s21_remove_matrix(&copy);
    }
  }

  return status;
}
