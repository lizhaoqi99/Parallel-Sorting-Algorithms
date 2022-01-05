#include <cilk/cilk.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SequentialQuickSort.h"

#define sequentialThreshold 50

int *prefixSumParallel(int *x, int length)
{
    // malloc and free frequently is time consuming 
    int *s = (int *)malloc(sizeof(int) * length);
    int *y = (int *)malloc(sizeof(int) * length / 2);
    int *z = (int *)malloc(sizeof(int) * length / 2);
    if (length == 1)
        s[0] = x[0];
    else
    {
        cilk_for (int i = 0; i < length / 2; ++i)
            y[i] = x[2 * i] + x[2 * i + 1];
        z = prefixSumParallel(y, length / 2);
        cilk_for (int i = 0; i < length; ++i)
        {
            if (i == 0)
                s[0] = x[0];
            else if ((i & 1) == 1)
                s[i] = z[i / 2];
            else
                s[i] = z[(i - 1) / 2] + x[i];
        }
    }
    free(z);
    free(y);
    return s;
}

int parallelPartition(int *input, int length)
{
    // select random pivot
    int pivot = (int)(rand() % length);
    int value = input[pivot];
    int pivotIndex = 0;
    if (length == 1)
        return pivotIndex;
    int *extra = (int*)malloc(sizeof(int) * length);
    int *lt = (int*)malloc(sizeof(int) * length);
    int *gt = (int*)malloc(sizeof(int) * length);
    cilk_for (int i = 0; i < length; ++i)
    {
        extra[i] = input[i];
        if (extra[i] < value)
            lt[i] = 1;
        else
            lt[i] = 0;
        if (extra[i] > value)
            gt[i] = 1;
        else
            gt[i] = 0;
    }
    lt = prefixSumParallel(lt, length);
    gt = prefixSumParallel(gt, length);
    // input[0..k-1] < value, input[k+1..length-1] > value
    int k = lt[length - 1];
    // also determine how many elements were equal to the pivot - needed for arrays that can contain duplicates
    int totalGreater = gt[length-1];
    int totalEqual = length-(k+totalGreater);
    input[k] = value;
    cilk_for (int i = 0; i < length; ++i)
    {
        if (extra[i] < value)
            input[lt[i] - 1] = extra[i];
        else if (extra[i] > value)
            input[k + (totalEqual-1)+ gt[i]] = extra[i];
    }
    cilk_for (int i = k; i < k+totalEqual; i++){
        input[i] = value;
    }
    return k;
}

void parallelQuickSortKernel(int *input, int length)
{
    // sequential quicksort base case
    if (length <= sequentialThreshold)
    {
        sequentialQuickSort(input, length);
        return;
    }
    int pivot = parallelPartition(input, length);
    int leftLength = pivot;
    int *rightArray = input + pivot + 1;
    int rightLength = length - leftLength - 1;
    cilk_spawn parallelQuickSortKernel(input, leftLength);
    parallelQuickSortKernel(rightArray, rightLength);
}

void parallelQuickSort(int *input, int length)
{
    parallelQuickSortKernel(input, length);
    cilk_sync;
}