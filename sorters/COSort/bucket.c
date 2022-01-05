#include <stdio.h>
#include <cilk/cilk.h>
#include "bucket.h"

struct Bucket createBucket(int* array, int offset, int length){
    struct Bucket* b = (struct Bucket*) malloc(sizeof(struct Bucket));
    b->array = array;
    b->offset = offset;
    b->length = length;
    return *b;
}

struct Bucket* createBuckets(int* bucketLengths, int* baseArray, int n){
    int h = ceil(sqrt(n));
    struct Bucket* buckets = (struct Bucket*) malloc(sizeof(struct Bucket)*h);
    int offset = 0;
    for (int i = 0; i < h; i++){
        buckets[i] = createBucket(baseArray, offset, bucketLengths[i]);
        offset += bucketLengths[i];
    }
    return buckets;
}

struct Bucket* Parallel_createBuckets(int* bucketLengths, int* bucketOffsets, int* baseArray, int n){
    int h = ceil(sqrt(n));
    struct Bucket* buckets = (struct Bucket*) malloc(sizeof(struct Bucket)*h);
    cilk_for (int i = 0; i < h; i++){
        buckets[i] = createBucket(baseArray, bucketOffsets[i], bucketLengths[i]);
    }
    return buckets;
}

void freeBuckets(struct Bucket* buckets, int numBuckets){
    // for (int i = 0; i < numBuckets; i++){
    //     free((buckets[i]));
    // }
    free(buckets);
}


void writeRangeToBucket(int* source, struct Bucket bucket, int srcIndex, int bucketIndex, int length){
    if (length > 0){
        memcpy(bucket.array+bucket.offset+bucketIndex, source+srcIndex, length*sizeof(int));
    }
}
