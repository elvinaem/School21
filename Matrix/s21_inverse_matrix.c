#include "s21_matrix.h"

int s21_inverse_matrix(s21_matrix *A, s21_matrix *result) {
  int status = OK;
  if (s21_check_matrix(A) != OK || result == NULL) {
    status = INCORRECT_MATRIX;
  } else if (s21_is_square(A) != OK || s21_inf_or_nan(A)) {
    status = CALCULATION_ERROR;
  } else {
    if (result->matrix != NULL) {
      s21_remove_matrix(result);
    }
    double det = 0.0;
    status = s21_determinant(A, &det);

    if (status == OK && fabs(det) > EPS) {
      s21_matrix complements = {0};
      s21_matrix transpose = {0};

      if (s21_calc_complements(A, &complements) == OK &&
          s21_transpose(&complements, &transpose) == OK) {
        double inv_det = 1.0 / det;
        status = s21_mult_number(&transpose, inv_det, result);
      } else {
        status = CALCULATION_ERROR;
      }

      s21_remove_matrix(&complements);
      s21_remove_matrix(&transpose);
    } else {
      status = CALCULATION_ERROR;
    }
  }
  return status;
}