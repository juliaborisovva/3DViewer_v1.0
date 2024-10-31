#ifndef _3D_VUVER_H_
#define _3D_VUVER_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_ARRAYS 3
#define OK 0
#define SOME_ERROR 1

typedef struct Matrix {
  float *matrix;
  unsigned int rows;
  unsigned int columns;
} matrix_t;

typedef struct facets {
  unsigned int *vertex;
  unsigned int num_of_vertex_in_facets;
} polygon_t;

typedef struct data {
  unsigned int count_of_vertex;
  unsigned int count_of_facets;
  matrix_t matrix_3d;
  polygon_t *polygons;
} main_struct;

void first_parcer_of_file(char *filename, main_struct *STRUC);

int create_matrix(int rows, int columns, matrix_t *result);

int remove_matrix(matrix_t *A);

int create_massive_of_polygons(int columns, polygon_t **result);

int remove_massive_of_polygons(polygon_t **A);

void v_case(char *charCode, int charCode_len, main_struct **STRUC,
            int *counter_of_col);

void second_parcer_of_file(char *filename, main_struct *STRUC);
#endif  // _3D_VUVER_H_