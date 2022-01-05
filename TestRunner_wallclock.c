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

    int i;
    // seed array with distinct integers
    for (i = 0; i < n; i++){
        array[i] = i;
    }
    // shuffle array into all permutations with equal probability
    for (int i = 0; i < n; i++){
        int swapIndex = ((int)rand())%(n-(i))+i;
        int temp = array[i];
        array[i] = array[swapIndex];
        array[swapIndex] = temp;
    }
    int *sorted;
    sorted = (int*)malloc(sizeof(int) * n);
    clock_gettime(CLOCK_MONOTONIC, &tic);
    SequentialMergeSort(array, sorted, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Array size: %d\n", n);
    printf("Sorted Sequentially: \n");
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(sorted, n) == 1){
        printf("Correct.\n");
    }else{
        printf("Incorrect.\n");
    }


    printf("\nParallel: \n");
    int *parsorted;
    parsorted = (int*)malloc(sizeof(int) * n);
    clock_gettime(CLOCK_MONOTONIC, &tic);
    ParallelMergeSort(array, parsorted, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(parsorted, n) == 1){
        printf("Correct.\n");
    }else{
        printf("Incorrect.\n");
    }



    printf("\nMulti-way: \n");
    // k can be equals to [2..n]
    int *kWaySorted, k = 7;
    kWaySorted = (int*)malloc(sizeof(int) * n);
    clock_gettime(CLOCK_MONOTONIC, &tic);
    kWayMergeSort(array, kWaySorted, k, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(sorted, n) && check(parsorted, n) && check(kWaySorted, n)) printf("Correct\n");
    else printf("Error\n");



    printf("Sequential Quicksort:\n");
    int *sqSorted;
    sqSorted = (int*)malloc(sizeof(int) * n);
    memcpy(sqSorted, array, sizeof(int) * n);
    clock_gettime(CLOCK_MONOTONIC, &tic);
    sequentialQuickSort(sqSorted, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(sqSorted, n) == 1){
        printf("Correct.\n");
    }else{
        printf("Incorrect.\n");
    }


    printf("Parallel Quicksort:\n");
    int *pqSorted;
    pqSorted = (int*)malloc(sizeof(int) * n);
    memcpy(pqSorted, array, sizeof(int) * n);
    clock_gettime(CLOCK_MONOTONIC, &tic);
    parallelQuickSort(pqSorted, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(pqSorted, n) == 1){
        printf("Correct.\n");
    }else{
        printf("Incorrect.\n");
    }
    if (checkArraysEqual(sorted, pqSorted, n) == 1){
        printf("Array contents correct.\n");
    }else{
        printf("Array contents incorrect.\n");
    }

    printf("Sequential COSort:\n");
    clock_gettime(CLOCK_MONOTONIC, &tic);
    int* output = COSort(array, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(output, n) == 1){
        printf("Correct.\n");
    }else{
        printf("Incorrect.\n");
    }
    printf("Contents check:\n");
    if (checkArraysEqual(output, sorted, n) == 1){
        printf("Correct contents.\n");
    }else{
        printf("Incorrect contents.\n");
    }
    

    printf("Parallel COSort:\n");
    clock_gettime(CLOCK_MONOTONIC, &tic);
    int* parOutput = Parallel_COSort(array, n);
    clock_gettime(CLOCK_MONOTONIC, &toc);
    printf("Time taken: %f seconds\n", (toc.tv_sec-tic.tv_sec) + (toc.tv_nsec - tic.tv_nsec) / 1000000000.0);
    if (check(parOutput, n) == 1){
        printf("Correct.\n");
    }else{
        printf("Incorrect.\n");
    }
    printf("Contents check:\n");
    if (checkArraysEqual(parOutput, sorted, n) == 1){
        printf("Correct contents.\n");
    }else{
        printf("Incorrect contents.\n");
    }

    return 0;
}