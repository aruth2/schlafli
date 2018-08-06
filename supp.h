#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int intcontains(int *list, int search, int numitems);
void printmatrix(double *matrix, int numberOfRows, int numberOfColumns, int stride, int rowmajor);
void printfloatmatrix(float *matrix, int numberOfRows, int numberOfColumns, int stride, int rowmajor);
int tokcount(char *line, char *del);
void matrixmultiplication(float *output, float *leftmatrix, float *rightmatrix, int m, int n, int o);

