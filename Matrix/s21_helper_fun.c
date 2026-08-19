#include "s21_matrix.h"

int s21_eq_matrix_size(s21_matrix *A, s21_matrix *B) {
  int status = SUCCESS;
  if (A->rows != B->rows || A->columns != B->columns) {
    status = FAILURE;
  }
  return status;
}

int s21_eq_fields(s21_matrix *A, s21_matrix *B) {
  int status = SUCCESS;
  if (A->columns != B->rows) {
    status = FAILURE;
  }
  return status;
}

int s21_check_matrix(s21_matrix *matrix) {
  int status = OK;
  if (matrix == NULL || matrix->matrix == NULL || matrix->rows < 1 ||
      matrix->columns < 1) {
    status = INCORRECT_MATRIX;
  }
  return status;
}

double s21_multiply_matrix(int i, int j, s21_matrix *A, s21_matrix *B) {
  double res = 0;

  for (int k = 0; k < B->rows; k++) {
    res += A->matrix[i][k] * B->matrix[k][j];
  }
  return res;
}

int s21_is_square(s21_matrix *matrix) {
  int status = OK;
  if (matrix->rows != matrix->columns) {
    status = CALCULATION_ERROR;
  }
  return status;
}

int s21_copy_matrix(s21_matrix *A, s21_matrix *copy) {
  int status = OK;
  if (s21_check_matrix(A) != OK || s21_check_matrix(copy) != OK) {
    return INCORRECT_MATRIX;
  } else if (!s21_eq_matrix_size(A, copy)) {
    return CALCULATION_ERROR;
  }

  for (int i = 0; i < A->rows && status == OK; i++) {
    for (int j = 0; j < A->columns; j++) {
      copy->matrix[i][j] = A->matrix[i][j];
    }
  }

  return status;
}

int s21_inf_or_nan(s21_matrix *A) {
  int status = OK;
  for (int i = 0; i < A->rows && status == OK; i++) {
    for (int j = 0; j < A->columns && status == OK; j++) {
      if (isinf(A->matrix[i][j]) || isnan(A->matrix[i][j])) {
        status = CALCULATION_ERROR;
      }
    }
  }
  return status;
}

int s21_triangular_form(s21_matrix *matrix, int *sign) {
  if (sign == NULL) {
    return INCORRECT_MATRIX;
  }
  int status = OK;
  int not_zero = 0;

  for (int i = 0; i < matrix->rows - 1; i++) {
    not_zero = s21_not_zero_row(matrix, i, i);
    if (not_zero == -1) {
      status = OK;
      *sign = 0;
    }
    if (not_zero != i && status == OK && *sign != 0) {
      s21_swap_rows(matrix, i, not_zero);
      *sign = -*sign;
    }
    if (status == OK && *sign != 0) {
      status = s21_eliminate_below_diagonal(matrix, i);
    }
  }

  return status;
}

void s21_swap_rows(s21_matrix *matrix, int row1, int row2) {
  double *temp = matrix->matrix[row1];
  matrix->matrix[row1] = matrix->matrix[row2];
  matrix->matrix[row2] = temp;
}

int s21_not_zero_row(s21_matrix *matrix, int start_row, int col) {
  int not_zero = -1;

  for (int row = start_row; row < matrix->rows && not_zero == -1; row++) {
    if (fabs(matrix->matrix[row][col]) > 1e-7) {
      not_zero = row;
    }
  }

  return not_zero;
}

int s21_eliminate_below_diagonal(s21_matrix *matrix, int current_row) {
  int status = OK;
  if (matrix == NULL || current_row < 0 || current_row > matrix->rows) {
    status = INCORRECT_MATRIX;
  } else if (fabs(matrix->matrix[current_row][current_row]) < EPS) {
    status = CALCULATION_ERROR;
  } else {
    for (int k = current_row + 1; k < matrix->rows; k++) {
      double multiplier = matrix->matrix[k][current_row] /
                          matrix->matrix[current_row][current_row];
      for (int j = current_row; j < matrix->columns; j++) {
        matrix->matrix[k][j] -= matrix->matrix[current_row][j] * multiplier;
      }
    }
  }
  return status;
}

void s21_algebraic_complements(s21_matrix *matrix, s21_matrix *result) {
  for (int i = 0; i < matrix->rows; i++) {
    for (int j = 0; j < matrix->columns; j++) {
      if ((i + j) % 2) {
        result->matrix[i][j] *= -1;
      }
    }
  }
}

void s21_minors_matrix(s21_matrix *minor, s21_matrix *matrix, int del_row,
                       int del_col) {
  for (int rows = 0, minor_row = 0; rows < matrix->rows; rows++) {
    for (int columns = 0, minor_col = 0; columns < matrix->columns; columns++) {
      if (rows != del_row && columns != del_col) {
        minor->matrix[minor_row][minor_col] = matrix->matrix[rows][columns];
        minor_col++;

        if (minor_col == minor->columns) {
          minor_row++;
          minor_col = 0;
        }
      }
    }
  }
}
