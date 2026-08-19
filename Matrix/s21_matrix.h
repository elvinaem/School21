#ifndef S21_MATRIX
#define S21_MATRIX

#include <dlfcn.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define SUCCESS 1
#define FAILURE 0
#define EPS 0.0000001

enum ERRORS { OK, INCORRECT_MATRIX, CALCULATION_ERROR };

typedef struct matrix_struct {
  double **matrix;
  int rows;
  int columns;
} s21_matrix;

// Основные функции
int s21_create_matrix(int rows, int columns, s21_matrix *result);
void s21_remove_matrix(s21_matrix *A);
int s21_eq_matrix(s21_matrix *A, s21_matrix *B);
int s21_sum_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result);
int s21_sub_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result);
int s21_mult_number(s21_matrix *A, double number, s21_matrix *result);
int s21_mult_matrix(s21_matrix *A, s21_matrix *B, s21_matrix *result);
int s21_transpose(s21_matrix *A, s21_matrix *result);
int s21_calc_complements(s21_matrix *A, s21_matrix *result);
int s21_determinant(s21_matrix *A, double *result);
int s21_inverse_matrix(s21_matrix *A, s21_matrix *result);

// Вспомогательные функции
int s21_eq_matrix_size(s21_matrix *A, s21_matrix *B);
int s21_check_matrix(s21_matrix *matrix);
int s21_eq_fields(s21_matrix *A, s21_matrix *B);
double s21_multiply_matrix(int i, int j, s21_matrix *A, s21_matrix *B);
int s21_is_square(s21_matrix *matrix);
int s21_copy_matrix(s21_matrix *A, s21_matrix *copy);
int s21_inf_or_nan(s21_matrix *A);
int s21_triangular_form(s21_matrix *A, int *det_sign);
void s21_swap_rows(s21_matrix *A, int row1, int row2);
int s21_not_zero_row(s21_matrix *matrix, int start_row, int col);
int s21_eliminate_below_diagonal(s21_matrix *matrix, int current_row);
void s21_algebraic_complements(s21_matrix *matrix, s21_matrix *result);
void s21_minors_matrix(s21_matrix *minor, s21_matrix *matrix, int del_row,
                       int del_col);

#endif