#include "ParallelMergeSort.h"
#include "SequentialMergeSort.h"
#include <cilk/cilk.h>

#define sequentialThreshold 50

void SemiParallelMergeSort(int* input, int* output, int length){
    if (length < sequentialThreshold){
        SequentialMergeSort(input, output, length);
        return;
    }
    // can't overflow, in case we are using really big arrays
    int mid = length/2;
    int* extra = (int*)malloc(length*sizeof(int));
    cilk_spawn SemiParallelMergeSort(input, extra, mid);
    SemiParallelMergeSort(input+mid, extra+mid, length-mid);
    cilk_sync;
    SequentialMerge(extra, output, mid, length);
    free(extra);
}
