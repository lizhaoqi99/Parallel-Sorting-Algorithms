#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cilk/cilk.h>
#include "sorters/MergeSort/SequentialMergeSort.c"
#include "sorters/MergeSort/ParallelMergeSort.c"
#include "sorters/KWayMergeSort/KWayMergeSort.c"
#include "sorters/KWayMergeSort/heap.c"
#include "sorters/QuickSort/SequentialQuickSort.c"
#include "sorters/QuickSort/ParallelQuickSort.c"
#include "sorters/COSort/SequentialCOSort.c"
#include "sorters/COSort/ParallelCOSort.c"
#include "sorters/FunnelSort/funnelsort.c"
#include "sorters/QuickSort/SemiParallelQuickSort.c"
#include "sorters/MergeSort/SemiParallelMergeSort.c"



// Check if arr is sorted
int check(int *arr, int n) {
    for (int i = 0; i < n - 1; ++ i) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}

int checkArraysEqual(int *A, int *B, int n){
    for (int i = 0; i < n; i++){
        if (A[i]!=B[i]){
            printf("Arrays not equal at index %d: %d vs %d\n", i, A[i], B[i]);
            return 0;
        }
    }
    return 1;
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: %s <length of array>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int *array;
    array = (int*)malloc(sizeof(int) * n);
    time_t t;
    srand((unsigned) time(&t));

    struct timespec tic, toc;
    double elapsed;

    int numTrials = 10; // average over trials
    float pMergeSortTimer = 0.0;
    float spMergeSortTimer = 0.0;
    float pQuickSortTimer = 0.0;
    float spQuickSortTimer = 0.0;
    float pCOSortTimer = 0.0;
    for (int trial = 0; trial < numTrials; trial++){
        // seed array with distinct integers
        for (int i = 0; i < n; i++){
            array[i] = i;
        }
        // shuffle array into all permutations with equal probability
        for (int i = 0; i < n; i++){
            int swapIndex = ((int)rand())%(n-(i))+i;
            int temp = array[i];
            array[i] = array[swapIndex];
            array[swapIndex] = temp;
        }

        printf("Running batch %d/%d\n", trial+1, numTrials);

        // get correct sorted output
        int *sorted;
        sorted = (int*)malloc(sizeof(int) * n);
        SequentialMergeSort(array, sorted, n);

        // int *parsorted;
        // parsorted = (int*)malloc(sizeof(int) * n);
        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // ParallelMergeSort(array, parsorted, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // pMergeSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(parsorted, sorted, n)){
        //     printf("Sorting failed in parallel merge sort\n");
        //     return 2;
        // }

        // int *spMergesorted;
        // spMergesorted = (int*)malloc(sizeof(int) * n);
        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // SemiParallelMergeSort(array, spMergesorted, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // spMergeSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(spMergesorted, sorted, n)){
        //     printf("Sorting failed in semi-parallel merge sort\n");
        //     return 2;
        // }

        // int *pqSorted;
        // pqSorted = (int*)malloc(sizeof(int) * n);
        // memcpy(pqSorted, array, sizeof(int) * n);
        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // parallelQuickSort(pqSorted, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // pQuickSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(pqSorted, sorted, n)){
        //     printf("Sorting failed in parallel quick sort\n");
        //     return 2;
        // }

        int *spqSorted;
        spqSorted = (int*)malloc(sizeof(int) * n);
        memcpy(spqSorted, array, sizeof(int) * n);
        clock_gettime(CLOCK_MONOTONIC, &tic);
        SemiParallelQuickSort(spqSorted, n);
        clock_gettime(CLOCK_MONOTONIC, &toc);
        spQuickSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        if (!checkArraysEqual(spqSorted, sorted, n)){
            printf("Sorting failed in semi-parallel quick sort\n");
            return 2;
        }


        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // int* parOutput = Parallel_COSort(array, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // pCOSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(parOutput, sorted, n)){
        //     printf("Sorting failed in parallel COSort\n");
        //     return 2;
        // }

        free(sorted);
        // free(parsorted);
        // free(spMergesorted);
        // free(pqSorted);
        free(spqSorted);
        // free(parOutput);
    }

    pMergeSortTimer /= numTrials;
    spMergeSortTimer /= numTrials;
    pQuickSortTimer /= numTrials;
    spQuickSortTimer /= numTrials;
    pCOSortTimer /= numTrials;


    printf("Array size: %d\n", n);
    printf("Parallel Merge Sort: %f\n", pMergeSortTimer);
    printf("Semi-Parallel Merge Sort: %f\n", spMergeSortTimer);
    printf("Parallel Quick Sort: %f\n", pQuickSortTimer);
    printf("Semi-Parallel Quick Sort: %f\n", spQuickSortTimer);
    printf("Parallel COSort: %f\n", pCOSortTimer);

 

    return 0;
}