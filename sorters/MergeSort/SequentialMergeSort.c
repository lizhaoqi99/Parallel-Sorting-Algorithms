#include "SequentialMergeSort.h"

void SequentialMergeSort(int* input, int* output, int length){
    if (length == 1){
        output[0] = input[0];
        return;
    }
    // can't overflow, in case we are using really big arrays
    int mid = length/2;
    int* extra = (int*)malloc(length*sizeof(int));
    SequentialMergeSort(input, extra, mid);
    SequentialMergeSort(input+mid, extra+mid, length-mid);
    SequentialMerge(extra, output, mid, length);
    free(extra);
}

void SequentialMerge(int* sorted, int* output, int mid, int maxExclusive){
    int lowIdx = 0;
    int highIdx = mid;
    int i;
    for (i = 0; i < maxExclusive; i++){
        if (lowIdx >= mid){
            output[i] = sorted[highIdx];
            highIdx++;
        }else if (highIdx >= maxExclusive){
            output[i] = sorted[lowIdx];
            lowIdx++;
        }else if (sorted[lowIdx] < sorted[highIdx]){
            output[i] = sorted[lowIdx];
            lowIdx++;
        }else{
            output[i] = sorted[highIdx];
            highIdx++;     
        }
    }
}

int BinarySearch(int* array, int value, int length){
    
    int low = 0;
    int high = length-1;
    while (low <= high){
        int mid = (low+high)/2;
        if (array[mid] >= value){
            high = mid-1;
        }else{
            low=mid+1;
        }
    }
    return low;
}
