#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cilk/cilk.h>
#include "ParallelCOSort.h"
#include "../MergeSort/ParallelMergeSort.h"
#include "../MergeSort/SequentialMergeSort.h"
#include "bucket.h"

// compute prefix sum over each row of the input, where input is a rows by cols matrix
// the UP/DOWN method from the paper https://www.cs.cmu.edu/~guyb/papers/BGVa09.pdf did not work for this
// instead, just parallelized the simple algorithm, though the span is not optimal
int* Parallel_prefixSum(int* input, int rows, int cols){
    int* output = (int*)malloc(sizeof(int)*rows*cols);
    cilk_for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (j > 0){
                output[i*cols+j] = output[i*cols+j-1] + input[i*cols+j-1];
            }else{
                output[i*cols] = 0;
            }
        }
    }

    return output;
}

// cache-optimal matrix transpose - but parallel, so take the cache-optimal part with a grain of salt
void Parallel_transpose(int* input, int* output, int minRow, int maxRow, int minCol, int maxCol, int inputRows, int inputCols){
    // if width=height=1, copy over
    if (maxRow-minRow == 1 && maxCol-minCol == 1){
        output[minCol*inputRows+minRow] = input[minRow*inputCols+minCol];
        return;
    }
    // if width > height, split input in half by vertical line, output in half by horizontal line
    if (maxRow-minRow < maxCol-minCol){
        cilk_spawn Parallel_transpose(input, output, minRow, maxRow, minCol, (minCol+maxCol)/2, inputRows, inputCols);
        Parallel_transpose(input, output, minRow, maxRow, (minCol+maxCol)/2, maxCol, inputRows, inputCols);
    }else{
        // otherwise, split input into top half and bottom half, output into left half and right half
        cilk_spawn Parallel_transpose(input, output, minRow, (minRow+maxRow)/2, minCol, maxCol, inputRows, inputCols);
        Parallel_transpose(input, output, (minRow+maxRow)/2, maxRow, minCol, maxCol, inputRows, inputCols);
    }
}

// M: numRowsxnumColsx2 matrix. L_T: numColsx(numRows+1) matrix, where the last column should all be 0s. 
// this takes the lengths from M, transposes them, and writes them to L_T.
// saves the time of creating a whole matrix L from M to just be transposed.
void Parallel_MtoL_transpose(int* M, int* L_T, int minRow, int maxRow, int minCol, int maxCol, int numRows, int numCols){
    // if one element, write
    if (maxRow-minRow == 1 && maxCol-minCol == 1){
        // if element is in the final column, write 0 instead of copying over
        if (minRow == numRows){
            L_T[minCol*(numRows+1)+minRow] = 0;
        }else{
            L_T[minCol*(numRows+1)+minRow] = M[minRow*2*numCols+2*minCol+1];
        }
        return;
    }
    // recursive split same as standard transpose
    if (maxRow-minRow < maxCol-minCol){
        cilk_spawn Parallel_MtoL_transpose(M, L_T, minRow, maxRow, minCol, (minCol+maxCol)/2, numRows, numCols);
        Parallel_MtoL_transpose(M, L_T, minRow, maxRow, (minCol+maxCol)/2, maxCol, numRows, numCols);
    }else{
        cilk_spawn Parallel_MtoL_transpose(M, L_T, minRow, (minRow+maxRow)/2, minCol, maxCol, numRows, numCols);
        Parallel_MtoL_transpose(M, L_T, (minRow+maxRow)/2, maxRow, minCol, maxCol, numRows, numCols);
    }
}

// For a given input array and partition set, creates partitionSize+1 ranges over the input.
// range i indices the start index in input for pivot p[i-1], and the length of that range (start for p[i] minus start for p[i-1]).
// Used to construct M in COSORT, by calling this method on each sorted subarray S.
int* Parallel_split(int* input, int* pivots, int inputLength, int partitionSize){
    int* output = malloc(sizeof(int)*(partitionSize+1)*2);
    // set first element
    output[0] = 0;
    output[1] = BinarySearch(input, pivots[0], inputLength);
    // set intermediate ranges
    cilk_for (int i = 0; i < partitionSize-1; i++){
        output[(i+1)*2] = BinarySearch(input, pivots[i], inputLength);
        output[(i+1)*2+1] = BinarySearch(input, pivots[i+1], inputLength)-output[(i+1)*2];
    }
    // set final range
    output[2*partitionSize] = BinarySearch(input, pivots[partitionSize-1], inputLength);
    output[2*partitionSize+1] = inputLength-output[2*partitionSize];
    return output;
}

// B-Transpose algorithm - takes in the sorted sets S to read from, the buckets B to write to, and the metadata T
// T[i_s][i_b] is a triple {x,y,z}, where x is the index in S[i_s] to read from, y is the index in B[i_b] to write to,
// and z is the amount of elements to write
// sCount, bCount, and h are used to determine boundaries within the sets and how many sets are being considered
void Parallel_B_transpose(int* S, struct Bucket* B, int* T, int i_s, int i_b, int sCount, int bCount, int h){
    if (sCount == 1 && bCount == 1){
        writeRangeToBucket(S+i_s*h, B[i_b], T[i_s*3*h+i_b*3], T[i_s*3*h+i_b*3+1], T[i_s*3*h+i_b*3+2]);
        return;
    }else if (sCount == 1){
        cilk_spawn Parallel_B_transpose(S, B, T, i_s, i_b, sCount, bCount/2, h);
        Parallel_B_transpose(S, B, T, i_s, i_b+(bCount/2), sCount, (bCount+1)/2, h);
    }else if (bCount == 1){
        cilk_spawn Parallel_B_transpose(S, B, T, i_s, i_b, sCount/2, bCount, h);
        Parallel_B_transpose(S, B, T, i_s+(sCount/2), i_b, (sCount+1)/2, bCount, h);
    }else{
        cilk_spawn Parallel_B_transpose(S, B, T, i_s, i_b, sCount/2, bCount/2, h);
        cilk_spawn Parallel_B_transpose(S, B, T, i_s, i_b+(bCount/2), sCount/2, (bCount+1)/2, h);
        cilk_spawn Parallel_B_transpose(S, B, T, i_s+(sCount/2), i_b, (sCount+1)/2, bCount/2, h);
        Parallel_B_transpose(S, B, T, i_s+(sCount/2), i_b+(bCount/2), (sCount+1)/2, (bCount+1)/2, h);
    }
}

// returns the set of pivots to be used to partition the data.
// takes in the fully sorted sequence of sets S and the total length.
// selects sqrt(n) elements to create pivots.
int* Parallel_getPivots(int* S, int n){
    // preparing arrays and variables
    int h = ceil(sqrt(n));
    int logn = ceil(log2(n));
    int sampleSize = n/logn;
    int sample[sampleSize];
    int spacing = logn;
    int initialoffset = 0;
    int additional = n%logn;
    // sample every lognth element from the sorted sets
    cilk_for (int i = 0; i < sampleSize; i++){
        sample[i] = S[initialoffset+spacing*i+(additional > i)];
    }
    // sort the sample with merge sort
    int sorted[sampleSize];
    ParallelMergeSort(sample, sorted, sampleSize);
    // want sqrt(n)-1 pivots to determine sqrt(n) bucket boundaries
    int* finalSample = (int*) malloc(sizeof(int)*(h-1));
    spacing = sampleSize/h;
    initialoffset = spacing;
    additional = sampleSize % h;
    // select set of pivots
    cilk_for (int i = 0; i < h-1; i++){
        finalSample[i] = sorted[initialoffset + spacing*i + (additional > i)];
    }
    return finalSample;
}

/*
 Parallel COSort: Cache-oblivious, parallel, sample sort
 1. Splits data into ~sqrt(n) sets of size sqrt(n)
 2. Sorts the sets recursively
 3. Selects pivots from the smaller sorted sets
 4. Determines how each set will be allocated between the pivots
 5. Creates a "bucket" for each pivot range
 6. Copies data from sorted sets into buckets
 7. Recursively sorts buckets
 8. Returns buckets in order
*/
int* Parallel_COSort(int* A, int n){
    // base case: n large enough such that sqrt(n) > log_2(n) to avoid issues with pivot selection
    if (n <= 20){
        int* B = (int*) malloc(n*sizeof(int));
        SequentialMergeSort(A, B, n);
        return B;
    }

    // determine sqrt(n) and the number of sorted sets S
    int h = ceil(sqrt(n));
    int numS = ceil(n/(h+0.0));

    // Sort each set of size sqrt(n) recursively
    int* S = malloc(sizeof(int)*n);
    cilk_for (int i = 0; i < n/h; i++){
        int* sorted = Parallel_COSort(A+i*h, h);
        memcpy(S+i*h, sorted, h*sizeof(int));
        // free(sorted);
    }
    if (n % h != 0){
        int* sorted = Parallel_COSort(A+(n-(n%h)), n%h);
        memcpy(S+(n-(n%h)), sorted, (n%h)*sizeof(int));
        // free(sorted);
    }

    // get pivots
    int* pivots = Parallel_getPivots(S, n);

    // generate the metadata array M to determine pivot ranges of each sorted set
    int* M = (int*) malloc(sizeof(int)*numS*h*2);
    cilk_for (int i = 0; i < n/h; i++){
        int* spl = Parallel_split(S+i*h, pivots, h, h-1);
        memcpy(M+i*h*2, spl, sizeof(int)*h*2);
        // free(spl);
    }
    if (n%h != 0){
        int* spl = Parallel_split(S+(n-(n%h)), pivots, n%h, h-1);
        memcpy(M+(numS-1)*h*2, spl, sizeof(int)*2*h);
        // free(spl);
    }

    // free pivots once we are done with it
    // free(pivots);

    // transpose the write lengths from M to get L_T
    int* L_T = (int*)malloc(sizeof(int)*(numS+1)*h);
    Parallel_MtoL_transpose(M, L_T, 0, numS+1, 0, h, numS, h);
    cilk_sync;

    // Take the prefix sums over the lengths to determine a) bucket offsets and b) bucket lengths
    int* O = Parallel_prefixSum(L_T, h, numS+1);

    // free L_T 
    // free(L_T);

    // Transpose O back to be in the same shape as M
    int* O_T = (int*) malloc(sizeof(int)*h*(numS+1));
    Parallel_transpose(O, O_T, 0, h, 0, numS+1, h, numS+1);
    cilk_sync;

    // free O now that it has been transposed
    // free(O);

    // construct complete metadata array T. 
    // T[i][j] = {x,y,z}, where:
    // x = index of S[i] to read from,
    // y = index of B[j] to write to,
    // z = amount of elements to copy 
    int* T = (int*) malloc(sizeof(int)*numS*h*3);
    cilk_for(int i = 0; i < numS; i++){
        cilk_for (int j = 0; j < h; j++){
            // T[i_s][i_b][0]: index in S[i_s] to start in bucket B[i_b]
            T[i*3*h+j*3] = M[i*2*h+2*j];
            // T[i_s][i_b][1]: index in B[i_b] to write elements from S[i_s] to
            T[i*3*h+j*3+1] = O_T[i*(h)+j]; 
            // T[i_s][i_b][2]: amount of items to write from S[i_s] to B[i_b]
            T[i*3*h+j*3+2] = M[i*2*h+2*j+1];
        }
    }
    
    // free M now that T has been constructed
    // free(M);

    // initialize buckets with correct lengths and offsets
    int* bucketArray = malloc(n*sizeof(int));
    int* bucketOffsets = Parallel_prefixSum(O_T+numS*h, 1, h);
    struct Bucket* B = Parallel_createBuckets(O_T+numS*h, bucketOffsets, bucketArray, n);

    // free bucket lengths
    // free(O_T);

    // copy elements from S into their corresponding buckets
    Parallel_B_transpose(S, B, T, 0, 0, numS, h, h);
    cilk_sync;

    // free T and S now that elements have been copied
    // free(T);
    // free(S);

    // recursively sort buckets
    int* output = malloc(n*sizeof(int));
    cilk_for (int i = 0; i < h; i++){
        if (B[i].length > 0){          
            // write recursive output into bucket array  
            int* recursiveOutput = COSort(B[i].array + B[i].offset, B[i].length);
            memcpy(output + B[i].offset, recursiveOutput, B[i].length*sizeof(int));
            free(recursiveOutput);
        }
    }
    // free buckets before returning
    // freeBuckets(B, h);
    // free(bucketArray);

    // return sorted output
    return output;
}