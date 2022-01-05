#ifndef HEAP_H
#define HEAP_H

struct Element
{
    int value;
    int arrayIndex; // [0 ... k-1]
};


struct Heap {
    struct Element *data;
    unsigned int currentSize;
    unsigned int heapCapacity;
};

struct Heap* createHeap(int heapCapacity);

void freeHeap(struct Heap *heap);

void insert(struct Heap *heap, struct Element *element);

struct Element* removeMin(struct Heap *heap);

#endif