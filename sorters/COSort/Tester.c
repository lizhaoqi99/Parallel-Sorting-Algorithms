#include <stdio.h>
#include <stdlib.h>
#include "../MergeSort/SequentialMergeSort.c"
#include "../MergeSort/ParallelMergeSort.c"
#include "SequentialCOSort.c"
#include "ParallelCOSort.c"


int check(int* array, int n){
    for (int i = 1; i < n; i++){
        if (array[i] < array[i-1]){
            return 1;
        }
    }
    return 0;
}

int checkSequentialMergesort(){
    int n = 25;
    int array[n];
    for (int i = 0; i < n; i++){
        array[i] = rand()%100;
    }
    int output[n];
    SequentialMergeSort(array, output, n);
    if (check(output, n) == 1){
        return 1;
    }
    for (int i = 0; i < n; i++){
        int val = array[i];
        int found = 0;
        for (int j = 0; j < n; j++){
            if (output[j] == val){
                found = 1;
            }
        }
        if (found == 0){
            return 2;
        }
    }
    return 0;

}

int checkPrefixSum(){
    int array[2][10];
    int prefix[]= {0, 0, 1, 3, 6, 10, 15, 21, 28, 36};
    for (int i = 0; i < 10; i++){
        array[0][i] = i;
        array[1][i] = 2*i;
    }
    int* summed = prefixSum(array, 2, 10);
    for (int i = 0; i < 10; i++){
        if (summed[i] != prefix[i]){
            
            return 1;
        }
        if (summed[i+10] != prefix[i]*2){
            return 1;
        }
    }
    return 0;
}

int checkParallelPrefixSum(){
    int testsToRun = 100;
    int ret = 0;
    for (int i = 0; i < testsToRun; i++){
        printf("Running test %d/%d\n", i+1, testsToRun);
        int n = rand()%500+10;
        int m = rand()%50 + 5;
        int array[m][n];
        for (int j = 0; j < m; j++){
            for (int k = 0; k < n; k++){
                array[j][k] = rand()%1000;
            }
        }
        int* expected = prefixSum(array, m, n);
        int* actual = Parallel_prefixSum(array, m, n);
        for (int j = 0; j < m*n; j++){
            if (expected[j] != actual[j]){
                printf("Error on row %d, column %d. Expected %d but was %d.", j/n, j%n, expected[j], actual[j]);
                ret = 1;
            }
        }
    }
    return ret;
}

int checkTranspose(){
    int rows = 5;
    int cols = 7;
    int* original = (int*) malloc(sizeof(int)*rows*cols);
    // 5 rows, 7 cols
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            original[i*cols+j] = rand();
        }
    }
    int* transposed = (int*) malloc(sizeof(int)*rows*cols);
    transpose(original, transposed, 0, rows, 0, cols, rows, cols);
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (original[i*cols+j] != transposed[j*rows+i]){
                return 1;
            }
        }
    }
    return 0;
}

int checkSplit(){
    int input[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int partitionSet[] = {3, 6};
    int expected[][2] = {{0, 3}, {3, 3}, {6, 4}};
    int* actual = split(input, partitionSet, 10, 2);

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 2; j++){
            if (expected[i][j] != actual[i*2+j]){
                return 1;
            }
        }
    }
    return 0;
}

int checkBinarySearch(){
    int input[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18};
    for (int i = 0; i < 20; i+=2){
        int index = BinarySearch(input, i, 10);
        if (index != i/2){
            return 1;
        }
    }
    for (int i = -1; i < 21; i+=2){
        int index = BinarySearch(input, i, 10);
        if (index != (i+1)/2){
            return 1;
        }
    }
    return 0;
}

int checkMtoL_transpose(){
    int n = 25;
    int array[] = {11, 42, 56, 67, 93, 19, 21, 29, 73, 84, 15, 24, 37, 70, 98, 13, 26, 56, 80, 91, 62, 70, 73, 81, 96};
    int pivots[] = {37, 56, 56, 73};
    int expectedSplits[][5][2] = {{{0, 1}, {1, 1}, {2, 0}, {2, 2}, {4,1}},
     {{0, 3}, {3, 0}, {3, 0}, {3, 0}, {3, 2}},
     {{0,2}, {2, 1}, {3, 0}, {3, 1}, {4, 1}},
     {{0, 2}, {2,0}, {2,0}, {2,1}, {3,2}},
     {{0,0}, {0,0}, {0,0}, {0,2}, {2,3}}};
    for (int i = 0; i < 5; i++){
        int* actual = split(array+i*5, pivots, 5, 4);
        for (int m = 0; m < 5; m++){
            for (int n = 0; n < 2; n++){
                if (expectedSplits[i][m][n] != actual[m*2+n]){
                    printf("Incorrect split in initial transpose\n");
                }
            }
        }
    }
    int h = 5;
    int* L_T = (int*) malloc(sizeof(int)*h*(h+1));
    // h rows, h+1 columns
    MtoL_transpose(&expectedSplits, L_T, 0, h+1, 0, h, h, h);
    int expected[][6] = {{1, 3, 2, 2, 0, 0}, {1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {2, 0, 1, 1, 2, 0}, {1, 2, 1, 2, 3, 0}};
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 6; j++){
            if (expected[i][j] != L_T[i*6+j]){
                return 1;
            }
        }
    }



    // for (int i = 0; i < 25; i++){
    //     array[i] = rand()%100;
    // }
    // for (int i = 0; i < 25; i+=5){
    //     int temp[5];
    //     SequentialMergeSort(array+i, temp, 5);
    //     for (int j = 0; j < 5; j++){
    //         array[i+j] = temp[j];
    //     }
    // }
    // printf("Initial array:\n");
    // for (int i = 0; i < 25; i++){
    //     printf("%d, ", array[i]);
    // }
    // printf("\n");
    // int* pivots = getPivots(array, 25);
    // // expect 4 pivots
    // for (int i = 0; i < 4; i++){
    //     printf("Pivot %d: %d\n", i, pivots[i]);
    // }
    // int sorted[n];
    // SequentialMergeSort(array, sorted, 25);
    // printf("Sorted array:\n");
    // for (int i = 0; i < 25; i++){
    //     printf("%d, ", sorted[i]);
    // }
    // printf("\n");

    return 0;
}

int checkTransposeAndSum(){
    int n = 25;
    int array[] = {11, 42, 56, 67, 93, 19, 21, 29, 73, 84, 15, 24, 37, 70, 98, 13, 26, 56, 80, 91, 62, 70, 73, 81, 96};
    int pivots[] = {37, 56, 56, 73};
    int* actualSplits = (int*) malloc(sizeof(int)*5*5*2);
    for (int i = 0; i < 5; i++){
        int* actual = split(array+i*5, pivots, 5, 4);
        memcpy(actualSplits+i*5*2, actual, sizeof(int)*5*2);
        free(actual);
    }
    int h = 5;
    int* L_T = (int*) malloc(sizeof(int)*h*(h+1));
    MtoL_transpose(actualSplits, L_T, 0, h+1, 0, h, h, h);
    int* summed = prefixSum(L_T, 5, 6);
    int expected[][6] = {{0, 1, 4, 6, 8, 8}, {0, 1, 1, 2, 2, 2}, {0, 0, 0, 0, 0, 0}, {0, 2, 2, 3, 4, 6}, {0, 1, 3, 4, 6, 9}};
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 6; j++){
           if (expected[i][j] != summed[i*6+j]){
               return 1;
           }
        }
    }
    return 0;
}

int checkSort(){
    int n = 24;
    int array[] = {11, 42, 56, 67, 93, 19, 21, 29, 73, 84, 15, 24, 37, 70, 98, 13, 26, 56, 80, 91, 62, 70, 73, 81};
    int* pivots = getPivots(array, n);
    // {37, 56, 56, 73};
    int* M = (int*) malloc(sizeof(int)*5*5*2);
    for (int i = 0; i < 4; i++){
        int* actual = split(array+i*5, pivots, 5, 4);
        memcpy(M+i*5*2, actual, sizeof(int)*5*2);
        free(actual);
    }
    int* actual = split(array+20, pivots, 4, 4);
    memcpy(M+4*5*2, actual, sizeof(int)*5*2);
    free(actual);
    int h = 5;
    int* L_T = (int*) malloc(sizeof(int)*h*(h+1));
    MtoL_transpose(M, L_T, 0, h+1, 0, h, h, h);
    int* O = prefixSum(L_T, 5, 6);
    int* O_T = (int*) malloc(sizeof(int)*h*(h+1));
    transpose(O, O_T, 0, h, 0, h+1, h, h+1);
    int* T = (int*) malloc(sizeof(int)*h*h*3);
    for(int i = 0; i < h; i++){ 
        for (int j = 0; j < h; j++){
            // T[i_s][i_b][0]: index in S[i_s] to start in bucket B[i_b]
            T[i*3*h+j*3] = M[i*2*h+2*j];
            // T[i_s][i_b][1]: index in B[i_b] to write elements from S[i_s] to
            T[i*3*h+j*3+1] = O_T[i*h+j];
            // T[i_s][i_b][2]: amount of items to write from S[i_s] to B[i_b]
            T[i*3*h+j*3+2] = M[i*2*h+2*j+1];
        }
    }

    int* output = (int*) malloc(sizeof(int)*n);
    struct Bucket* B = createBuckets(O_T+h*h, output, n);
    printf("Bucket sizes:\n");
    for (int i = 0; i < h; i++){
        printf("%d, ", O_T[h*h+i]);
    }
    printf("\n");

    B_transpose(array, B, T, 0, 0, h, h, h);

    printf("Re-ordered output array:\n");
    for (int i = 0; i < n; i++){
        printf("%d, ", output[i]);
    }
    printf("\n");
    for (int i = 0; i < h; i++){
        printf("Bucket %d offset: %d\n", i, B[i].offset);
        printf("Bucket %d length: %d\n", i, B[i].length);
    }

    int* bucketLengths = O_T+h*h;

    // for (int i = 0; i < h; i++){
    //     for (int j = 0; j < h; j++){
    //         printf("T[%d][%d]: {%d, %d, %d}\n", i, j, T[i*3*h+j*3], T[i*3*h+j*3+1], T[i*3*h+j*3+2]);
    //     }
    // }
    for (int i = 0; i < h; i++){
        if (bucketLengths[i] >0){
            int* sorted = (int*) malloc(sizeof(int)*bucketLengths[i]);
            SequentialMergeSort(output+B[i].offset, sorted, bucketLengths[i]);
            memcpy(output+B[i].offset, sorted, sizeof(int)*bucketLengths[i]);
            free(sorted);
        }
    }
    printf("Sorted array:\n");
    for (int i = 0; i < n; i++){
        printf("%d, ", output[i]);
    }
    printf("\n");
    
    return check(output, n);
}



int main(){
    srand(1);
    // printf("Sequential Mergesort test: %d\n", checkSequentialMergesort());
    // printf("Errors in prefixSum: %d\n", checkPrefixSum());
    // printf("Errors in transpose: %d\n", checkTranspose());
    // printf("Errors in split: %d\n", checkSplit());
    // printf("Errors in Binary search: %d\n", checkBinarySearch());
    // printf("Errors in MtoL_T: %d\n", checkMtoL_transpose());
    // printf("Error in sum after transpose: %d\n", checkTransposeAndSum());
    // printf("Error in sort: %d\n", checkSort());
    printf("Error in parallel prefix sum: %d\n", checkParallelPrefixSum());
    return 0;
}