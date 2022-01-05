#include "bucket.h"

int* COSort(int* input, int n);

void B_transpose(int*  S, struct Bucket* B, int* T, int i_s, int i_b, int sCount, int bCount, int h);

void transpose(int* input, int* output, int minRow, int maxRow, int minCol, int maxCol, int inputRows, int inputCols);

int* split(int* input, int* partitionSet, int inputLength, int partitionSize);

int* prefixSum(int* input, int width, int height);

int* getPivots(int* S, int n);

void MtoL_transpose(int* M, int* L_T, int minRow, int maxRow, int minCol, int maxCol, int numRows, int numCols);
