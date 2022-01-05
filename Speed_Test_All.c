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
    float sMergeSortTimer = 0.0;
    float pMergeSortTimer = 0.0;
    float kMergeSortTimer = 0.0;
    float spMergeSortTimer = 0.0;
    float sQuickSortTimer = 0.0;
    float pQuickSortTimer = 0.0;
    float spQuickSortTimer = 0.0;
    float funnelSortTimer = 0.0;
    float sCOSortTimer = 0.0;
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

        int *sorted;
        sorted = (int*)malloc(sizeof(int) * n);
        clock_gettime(CLOCK_MONOTONIC, &tic);
        SequentialMergeSort(array, sorted, n);
        clock_gettime(CLOCK_MONOTONIC, &toc);
        sMergeSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        if (!check(sorted, n)){
            printf("Sorting failed in sequential merge sort\n");
            return 2;
        }

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

        // int *kWaySorted, k = 16;
        // kWaySorted = (int*)malloc(sizeof(int) * n);
        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // kWayMergeSort(array, kWaySorted, k, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // kMergeSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(kWaySorted, sorted, n)){
        //     printf("Sorting failed in k-way merge sort\n");
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
      
        // int *sqSorted;
        // sqSorted = (int*)malloc(sizeof(int) * n);
        // memcpy(sqSorted, array, sizeof(int) * n);
        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // sequentialQuickSort(sqSorted, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // sQuickSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(sqSorted, sorted, n)){
        //     printf("Sorting failed in sequential quick sort\n");
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

        // int *spqSorted;
        // spqSorted = (int*)malloc(sizeof(int) * n);
        // memcpy(spqSorted, array, sizeof(int) * n);
        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // SemiParallelQuickSort(spqSorted, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // spQuickSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(spqSorted, sorted, n)){
        //     printf("Sorting failed in semi-parallel quick sort\n");
        //     return 2;
        // }

        int *funnelSorted;
        funnelSorted = (int*)malloc(sizeof(int) * n);
        memcpy(funnelSorted, array, sizeof(int) * n);
        clock_gettime(CLOCK_MONOTONIC, &tic);
        funnel_sort(funnelSorted, n, sizeof(int), comparator);
        clock_gettime(CLOCK_MONOTONIC, &toc);
        funnelSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        if (!checkArraysEqual(funnelSorted, sorted, n)){
            printf("Sorting failed in funnel sort. Should have been:\n");
            for (int i = 0; i < 50; i++){
                printf("%d, ", sorted[i]);
            }
            printf("\nBut was:\n");
            for (int i = 0; i < 50; i++){
                printf("%d, ", funnelSorted[i]);
            }
            printf("\n");
            return 2;
        }

        // clock_gettime(CLOCK_MONOTONIC, &tic);
        // int* seqCOS = COSort(array, n);
        // clock_gettime(CLOCK_MONOTONIC, &toc);
        // sCOSortTimer += (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0;
        // if (!checkArraysEqual(seqCOS, sorted, n)){
        //     printf("Sorting failed in sequential COSort\n");
        //     return 2;
        // }

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
        // free(kWaySorted);
        // free(spMergesorted);
        // free(sqSorted);
        // free(pqSorted);
        // free(spqSorted);
        free(funnelSorted);
        // free(seqCOS);
        // free(parOutput);
    }

    sMergeSortTimer /= numTrials;
    pMergeSortTimer /= numTrials;
    kMergeSortTimer /= numTrials;
    spMergeSortTimer /= numTrials;
    sQuickSortTimer /= numTrials;
    pQuickSortTimer /= numTrials;
    spQuickSortTimer /= numTrials;
    funnelSortTimer /= numTrials;
    sCOSortTimer /= numTrials;
    pCOSortTimer /= numTrials;


    printf("Array size: %d\n", n);
    printf("Sequential Merge Sort: %f\n", sMergeSortTimer);
    printf("Parallel Merge Sort: %f\n", pMergeSortTimer);
    printf("K-Way Merge Sort: %f\n", kMergeSortTimer);
    printf("Semi-Parallel Merge Sort: %f\n", spMergeSortTimer);
    printf("Sequential Quick Sort: %f\n", sQuickSortTimer);
    printf("Parallel Quick Sort: %f\n", pQuickSortTimer);
    printf("Semi-Parallel Quick Sort: %f\n", spQuickSortTimer);
    printf("Funnel Sort: %f\n", funnelSortTimer);
    printf("Sequential COSort: %f\n", sCOSortTimer);
    printf("Parallel COSort: %f\n", pCOSortTimer);

 

    return 0;
}