#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void arraySwap(int* input, int a, int b){
    int temp = input[a];
    input[a] = input[b];
    input[b] = temp;
}

int sequentialPartition(int* input, int length){
    // select random pivot
    int pivot = (int)(rand()%length);
    int value = input[pivot];
    // move pivot to end of array
    arraySwap(input, pivot, length-1);
    int pivotIndex = 0;
    for (int i = 0; i < length-1; i++){
        // if less than or equal to pivot value
        if (input[i] <= value){
            // move to first half of array
            arraySwap(input, i, pivotIndex);
            // at the end, the pivot should be at the correct index
            pivotIndex++;
        }
    }
    arraySwap(input, pivotIndex, length-1);
    return pivotIndex;
}


// sorts in-place. need to copy array before calling.
void sequentialQuickSort(int* input, int length){
    if (length <= 1){
        return;
    }
    // do the partition step
    int pivotLocation = sequentialPartition(input, length);
    // length of the left array = index of the pivot
    // start of the left array = start of the input
    int leftLength = pivotLocation;
    // start of the right array: index after the pivot
    int* rightArray = input+pivotLocation+1;
    // length of the right array: original minus the length of the left, minus 1 for the pivot spot
    int rightLength = length-leftLength-1;
    sequentialQuickSort(input, leftLength);
    sequentialQuickSort(rightArray, rightLength);
}