#include <cilk/cilk.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "SequentialQuickSort.h"

#define sequentialThreshold 50

void SemiParallelQuickSortKernel(int *input, int length)
{
    // sequential quicksort base case
    if (length <= sequentialThreshold)
    {
        sequentialQuickSort(input, length);
        return;
    }
    int pivot = sequentialPartition(input, length);
    int leftLength = pivot;
    int *rightArray = input + pivot + 1;
    int rightLength = length - leftLength - 1;
    cilk_spawn SemiParallelQuickSortKernel(input, leftLength);
    SemiParallelQuickSortKernel(rightArray, rightLength);
}

void SemiParallelQuickSort(int *input, int length)
{
    SemiParallelQuickSortKernel(input, length);
    cilk_sync;
}