#ifndef BUCKET_H
#define BUCKET_H

struct Bucket {
    int* array;
    int offset;
    int length;
};


struct Bucket createBucket(int* array, int offset, int length);

struct Bucket* createBuckets(int* bucketLengths, int* baseArray, int n);

struct Bucket* Parallel_createBuckets(int* bucketLengths, int* bucketOffsets, int* baseArray, int n);

void freeBuckets(struct Bucket* b, int numBuckets);

void writeRangeToBucket(int* source, struct Bucket bucket, int srcIndex, int bucketIndex, int length);

#endif

