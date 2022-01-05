#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "heap.h"

// partition: indicates max indices (exclusive) of k subarrays of sorted
// ranges of subarrays of sorted: [0, partition[0]-1], [partition[0], partition[1]-1], ... [partition[k-2], partition[k-1]-1]  (partition[k-1] = length)
void kWayMerge(int *sorted, int *result, int k, int *partition) {
    // Initialize the heap,  and insert the first elements from k subarrays in the heap
    struct Heap* heap = createHeap(k);
    int indices[k]; // indices[i] = the current index of ith subarray
    int finishedCount = 0; // count(indices[i] == partition[i])
    int resultIdx = 0;
    // Init indices (starting points of subarrays)
    for (int i = 0; i < k; ++ i) {
        if (i == 0) indices[i] = 0;
        else indices[i] = partition[i - 1];
    }
    for (int i = 0; i < k; ++ i) {
        struct Element *element = (struct Element*)malloc(sizeof(struct Element));
        element->value = sorted[indices[i] ++ ];
        element->arrayIndex = i;
        insert(heap, element);
        if (indices[i] == partition[i]) ++ finishedCount;
    }
    // Find and remove the minimum element, and insert a new element from the same subarray
    // Execute until all elements have been merged
    while (1) {
        if (finishedCount == k) {
            while (heap->currentSize != 0) result[resultIdx ++ ] = removeMin(heap)->value;
            break;
        }
        struct Element *element = removeMin(heap);
        int curArray = element->arrayIndex;
        result[resultIdx ++ ] = element->value;
        // Try to insert a new element from the same subarray
        if (indices[curArray] < partition[curArray]) {
            struct Element *element = (struct Element*)malloc(sizeof(struct Element));
            element->value = sorted[indices[curArray] ++ ];
            element->arrayIndex = curArray;
            insert(heap, element);
            if (indices[curArray] == partition[curArray]) ++ finishedCount;
        }
        // There is no element in the same subarray, so we need to find another one
        else {
            while (1) {
                curArray = (curArray + 1) % k;
                if (indices[curArray] == partition[curArray]) continue;
                else {
                    struct Element *element = (struct Element*)malloc(sizeof(struct Element));
                    element->value = sorted[indices[curArray] ++ ];
                    element->arrayIndex = curArray;
                    insert(heap, element);
                    if (indices[curArray] == partition[curArray]) ++ finishedCount;
                    break;
                }
            }

        }
    }
    freeHeap(heap);
}

void kWayMergeSort(int *input, int *result, int k, int length) {
    int i;
    if (length == 1) {
        result[0] = input[0];
        return;
    }
    if (length < k){
        kWayMergeSort(input, result, length, length);
        return;
    }
    int* extra = (int*)malloc(length*sizeof(int));
    int lengthOfSubarray = length / k;
    int partition[k];
    for (i = 0; i < k; ++ i) {
        if (i == 0) partition[i] = lengthOfSubarray;
        else {
            if (i == k - 1 && partition[i - 1] + lengthOfSubarray < length) partition[i] = length;
            else partition[i] = partition[i - 1] + lengthOfSubarray;
        }
    }
    for (i = 0; i < k; ++ i) {
        if (i == 0) kWayMergeSort(input, extra, k, partition[i]);
        else kWayMergeSort(input + partition[i - 1], extra + partition[i - 1], k, partition[i] - partition[i - 1]);
    }
    kWayMerge(extra, result, k, partition);
    free(extra);
}