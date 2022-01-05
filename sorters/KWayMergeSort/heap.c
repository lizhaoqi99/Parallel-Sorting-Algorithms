#include <stdio.h>
#include <malloc.h>
#include "heap.h"

struct Heap* createHeap(int heapCapacity) {
    struct Heap* heap = (struct Heap*)malloc(sizeof(struct Heap));
    if (heap == NULL) return NULL;
    heap->data = (struct Element*)malloc(sizeof(struct Element) * heapCapacity);
    if (heap->data == NULL) {
        free(heap);
        return NULL;
    }
    heap->currentSize = 0;
    heap->heapCapacity = heapCapacity;
    return heap;
}

void freeHeap(struct Heap *heap) {
    if (heap != NULL) {
        if (heap->data != NULL) {
            free(heap->data);
            heap->data = NULL;
        }
        free(heap);
    }
}

void swap(struct Element *a, struct Element *b) {
    struct Element temp = *a; *a = *b; *b = temp;
}

void insert(struct Heap *heap, struct Element *element) {
    int position = 0, parentPos;
    if (heap->currentSize >= heap->heapCapacity) {
        printf("error: insert\n");
        return;
    }
    heap->data[heap->currentSize ++ ] = *element;
    // Move the element up in the heap until it reaches the correct level (sift up)
    position = heap->currentSize;
    while (position >> 1) {
        parentPos = position >> 1;
        if (heap->data[parentPos - 1].value > heap->data[position - 1].value) {
            swap(&heap->data[parentPos - 1], &heap->data[position - 1]);
            position = parentPos;
        }
        else break;
    }
}

struct Element* removeMin(struct Heap *heap) {
    struct Element *element = (struct Element*)malloc(sizeof(struct Element));
    element->value = heap->data[0].value;
    element->arrayIndex = heap->data[0].arrayIndex;
    int position = 1, childPos;
    if (heap->currentSize <= 0) {
        printf("error: removeMin\n");
        return NULL;
    }
    heap->data[0] = heap->data[ -- heap->currentSize];
    // Sift down
    while ((position << 1) <= heap->currentSize) {
        // Find the smaller child
        childPos = position << 1;
        if (childPos + 1 <= heap->currentSize) {
            if (heap->data[childPos - 1].value > heap->data[childPos].value) ++ childPos;
        }
        // Swap the current node and the smaller child if needed
        if (heap->data[childPos - 1].value < heap->data[position - 1].value) {
            swap(&heap->data[childPos - 1], &heap->data[position - 1]);
            position = childPos;
        }
        else break;
    }
    return element;
}