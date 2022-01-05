#include "ParallelMergeSort.h"
#include "SequentialMergeSort.h"
#include <cilk/cilk.h>

#define sequentialThreshold 50

void ParallelMergeSort(int* input, int* output, int length){
    if (length < sequentialThreshold){
        SequentialMergeSort(input, output, length);
        return;
    }
    // can't overflow, in case we are using really big arrays
    int mid = length/2;
    int* extra = (int*)malloc(length*sizeof(int));
    cilk_spawn ParallelMergeSort(input, extra, mid);
    ParallelMergeSort(input+mid, extra+mid, length-mid);
    cilk_sync;
    ParallelMerge(extra, extra+mid, output, mid, length-mid);
    free(extra);
}

void ParallelMerge(int* A, int* B, int* output, int Alen, int Blen){
    if (Alen < Blen){
        ParallelMerge(B, A, output, Blen, Alen);
    }else{
        // Alen >= Blen, so if Alen is 1, Blen must be 0 or 1
        // base case
        if (Alen == 1){
            if (Blen == 0){
                output[0] = A[0];
            }else{
                if (A[0] < B[0]){
                    output[0] = A[0];
                    output[1] = B[0];
                }else{
                    output[0] = B[0];
                    output[1] = A[0];
                }
            }
        }
        // recursive calls - split w/ binary search
        else {
            int Amid = Alen/2;
            int AmidValue = A[Amid];
            int Bmid = BinarySearch(B, AmidValue, Blen); 
            cilk_spawn ParallelMerge(A, B, output, Amid, Bmid);
            ParallelMerge(A+Amid, B+Bmid, output+Amid+Bmid, Alen-Amid, Blen-Bmid);
            cilk_sync;
        }
    }
}
