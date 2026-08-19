#include "s21_matrix.h"

int s21_create_matrix(int rows, int columns, s21_matrix *result) {
  int status = OK;

  if (result == NULL || rows < 1 || columns < 1) {
    status = INCORRECT_MATRIX;
  } else {
    result->rows = rows;
    result->columns = columns;
    result->matrix = calloc(rows, sizeof(double *));

    if (result->matrix == NULL) {
      status = INCORRECT_MATRIX;
      result->rows = 0;
      result->columns = 0;
    } else {
      for (int i = 0; i < rows && status == OK; i++) {
        result->matrix[i] = calloc(columns, sizeof(double));

        if (result->matrix[i] == NULL) {
          status = INCORRECT_MATRIX;
          s21_remove_matrix(result);
        }
      }
    }
  }
  return status;
}