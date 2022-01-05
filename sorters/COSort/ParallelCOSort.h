#include "bucket.h"

int* ParallelCOSort(int* input, int n);

void Parallel_B_transpose(int*  S, struct Bucket* B, int* T, int i_s, int i_b, int sCount, int bCount, int h);

void Parallel_transpose(int* input, int* output, int minRow, int maxRow, int minCol, int maxCol, int inputRows, int inputCols);

int* Parallel_split(int* input, int* partitionSet, int inputLength, int partitionSize);

int* Parallel_prefixSum(int* input, int width, int height);

int* Parallel_getPivots(int* S, int n);

void Parallel_MtoL_transpose(int* M, int* L_T, int minRow, int maxRow, int minCol, int maxCol, int numRows, int numCols);
