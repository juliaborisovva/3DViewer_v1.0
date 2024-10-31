#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void first_parcer_of_file(char *filename, main_struct *STRUC) {
  FILE *f;
  f = fopen(filename, "r");
  char charCode[1024];
  while (!feof(f)) {
    if (fgets(charCode, 255, f)) {
      if ((*charCode == 'f') && (*(charCode + 1) == ' ')) {
        STRUC->count_of_facets++;
      } else if ((*charCode == 'v') && (*(charCode + 1) == ' ')) {
        STRUC->count_of_vertex++;
      }
    }
  }
  fclose(f);
}

////////////////////////////////////////////////////////////

int create_matrix(int rows, int columns, matrix_t *result) {
  result->matrix = NULL;
  result->rows = 0;
  result->columns = 0;
  int res = 0;
  if (rows > 0 && columns > 0) {
    result->matrix =
        (float *)calloc((size_t)rows * columns, (size_t)sizeof(float));
    result->rows = rows;
    result->columns = columns;
  } else {
    res = 1;
  }
  return res;
}

int remove_matrix(matrix_t *A) {
  int result = OK;
  if (A->matrix != NULL) {
    free(A->matrix);
    A->columns = 0;
    A->rows = 0;
    A->matrix = NULL;
  } else {
    result = SOME_ERROR;
  }
  return result;
}

int create_massive_of_polygons(int columns, polygon_t **result) {
  *result = NULL;
  int res = 0;
  if (columns > 0) {
    *result = (polygon_t *)calloc((size_t)columns, (size_t)sizeof(polygon_t));
    (*result)->num_of_vertex_in_facets = columns;
  } else {
    res = 1;
  }
  return res;
}

int remove_massive_of_polygons(polygon_t **A) {
  int result = OK;
  if (*A != NULL) {
    for (unsigned int r = 0; r < (*A)->num_of_vertex_in_facets; r++) {
      free((*A)[r].vertex);
    }
    free(*A);
    *A = NULL;
  } else {
    result = SOME_ERROR;
  }
  return result;
}

void v_case(char *charCode, int charCode_len, main_struct **STRUC,
            int *counter_of_col) {
  int counter_of_ch = 1;

  while (counter_of_ch < charCode_len) {
    if (charCode[counter_of_ch] == '\n') break;

    while (charCode[counter_of_ch] != ' ') {
      counter_of_ch++;
    }

    counter_of_ch++;
    char double_in_str[255];
    int counter_of_num = 0;

    while (charCode[counter_of_ch] != ' ') {
      if (charCode[counter_of_ch] == '\0' || charCode[counter_of_ch] == '\n')
        break;
      double_in_str[counter_of_num++] = *(charCode + counter_of_ch++);
    }

    double_in_str[counter_of_num] = '\0';
    (*STRUC)->matrix_3d.matrix[(*counter_of_col)++] =
        (float)atof(double_in_str);
  }
}

void second_parcer_of_file(char *filename, main_struct *STRUC) {
  FILE *f;
  f = fopen(filename, "r");

  char charCode[1024];
  int matrix_row = 1;
  int polygon_col = 1;
  int number_of_polygon = 1;

  int counter_of_col = NUM_OF_ARRAYS;

  STRUC->matrix_3d.matrix[0] = 0;
  STRUC->matrix_3d.matrix[1] = 0;
  STRUC->matrix_3d.matrix[2] = 0;

  int line_counter = 1;

  while (!feof(f)) {
    if (fgets(charCode, 1024, f)) {
      if ((*charCode == 'f') && (*(charCode + 1) == ' ')) {
        unsigned int charCode_len = strlen(charCode);
        unsigned int facets_and_vertex = 0;

        for (unsigned int i = 1; i < charCode_len; i++) {
          if ((*(charCode + i)) == '\n' || (*(charCode + i)) == '\0' ||
              (*(charCode + i)) == '\r') {
            break;
          } else if ((*(charCode + i)) == ' ') {
            continue;
          } else {
            while ((*(charCode + i)) != ' ') {
              if ((*(charCode + i)) == '\n' || (*(charCode + i)) == '\0' ||
                  (*(charCode + i)) == '\r')
                break;
              i++;
            }
            facets_and_vertex++;
          }
        }
        (STRUC)->polygons[((polygon_col))++].num_of_vertex_in_facets =
            facets_and_vertex + 1;

        (STRUC)->polygons[(number_of_polygon)].vertex =
            (unsigned int *)calloc((size_t)(STRUC)
                                       ->polygons[(number_of_polygon)]
                                       .num_of_vertex_in_facets,
                                   (size_t)sizeof(int));

        char tmp_to_int[255] = {};

        int cellar_count = 0;

        for (unsigned int i = 1; i < charCode_len; i++) {
          if (*(charCode + i) == ' ') {
            continue;
          } else if ((*(charCode + i) == '\n') || (*(charCode + i) == '\0') ||
                     (*(charCode + i)) == '\r') {
            break;
          } else {
            int tmp_counter = 0;

            while (*(charCode + i) != ' ') {
              if ((*(charCode + i) == '\n') || (*(charCode + i) == '\0') ||
                  (*(charCode + i) == '\r')) {
                break;
              } else if (*(charCode + i) == '/') {
                while ((*(charCode + i + 1) != ' ')) {
                  if ((*(charCode + i + 1) == '\n') ||
                      (*(charCode + i + 1) == '\0') ||
                      (*(charCode + i + 1)) == '\r') {
                    break;
                  }
                  i++;
                }
                i++;
              } else {
                *(tmp_to_int + tmp_counter++) = *(charCode + i);
                i++;
              }
            }
            *(tmp_to_int + tmp_counter) = '\0';
            int vertex_value = atoi(tmp_to_int);
            if (vertex_value > 0) {
              (STRUC)->polygons[(number_of_polygon)].vertex[cellar_count++] =
                  (unsigned int)vertex_value;
            } else if (line_counter > (vertex_value * -1)) {
              (STRUC)->polygons[(number_of_polygon)].vertex[cellar_count++] =
                  (unsigned int)(line_counter + vertex_value);
            } else {
              printf("Error!\n");
            }
          }
        }

        (STRUC)->polygons[(number_of_polygon)].vertex[cellar_count] =
            (STRUC)->polygons[(number_of_polygon)].vertex[0];
        number_of_polygon++;
      } else if ((*charCode == 'v') && (*(charCode + 1) == ' ')) {
        int charCode_len = strlen(charCode);
        v_case(charCode, charCode_len, &STRUC, &counter_of_col);
        matrix_row++;
        line_counter++;
      }
    }
  }
  fclose(f);
}
