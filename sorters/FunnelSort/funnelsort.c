/* Adopted from https://github.com/Shmelnick/Funnelsort and adjusted accordingly */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#define L 64 // cache-line size
#include "funnelsort.h"

int comparator(const void *a, const void *b) {
    return (*(int *)a < *(int *)b ? -1 : 1);
}

funnel *create_funnel(void *arr, int size, size_t nmemb, cmp_t cmp)
{
	// initialize funnel
	funnel *f = (funnel *)malloc(sizeof(funnel));
	f->size = size;
	f->begin_sort_ptr = arr;
	f->num_subarr = 0;
	f->nmemb = nmemb;
	f->cmp = cmp;
	f->buff = malloc(L); // fixed buffer length
	f->stream_size = size;
	f->num_ele_buff = 0;	   // initialize the buffer to be empty
	f->tail = f->buff;		   // tail points to the first element in the buffer
	f->head = f->buff + nmemb; // position the head of the buffer so that when writing
							   // to the buffer, the head and tail would match

	if ((f->nmemb) * size <= L)
	{ // base case, we reached a leaf node -> write the elements and sort them
		memcpy(f->buff, f->begin_sort_ptr, f->size * nmemb);
		qsort(f->buff, f->size, nmemb, cmp);
		// adjust pointers
		f->head = f->buff;
		f->tail = f->buff + (size - 1) * nmemb;
		f->num_ele_buff = size;
	}
	else
	{
		int subarr_length = (int)floor(pow(size, 2 / 3.0)); // each subarray has length n^(2/3)
		// In the pseudocode we split an array with size n into n^(1/3) subarrays
		// here we need to round it so that the number of subarrays is an integer
		f->num_subarr = (int)ceil((double)size / subarr_length);
		int n_right = size - subarr_length * (f->num_subarr - 1);
		f->sub_funnels = (funnel **)malloc(sizeof(funnel *) * f->num_subarr);

		int i;
		for (i = 0; i < f->num_subarr - 1; i++) {
			f->sub_funnels[i] = create_funnel(f->begin_sort_ptr + subarr_length * i * nmemb, subarr_length, nmemb, cmp);
		}

		f->sub_funnels[i] = create_funnel(f->begin_sort_ptr + subarr_length * i * nmemb, n_right, nmemb, cmp);
	}

	return f;
}

void *pop_buffer(funnel *f)
{
	void *pop_ele = f->head; // pop the head
	if (f->head == f->buff + ((L / f->nmemb) - 1) * f->nmemb) {
		f->head = f->buff; // shift head pointer
	} else {
		f->head += f->nmemb; // shift head pointer
	}
	
	f->stream_size -= 1;
	f->num_ele_buff -= 1;
	return pop_ele;
}

void push_buffer(funnel *f, void *val_ptr)
{
	// append the input value to the tail of the funnel
	if (f->tail == f->buff + ((L / f->nmemb) - 1) * f->nmemb) {
		f->tail = f->buff; // shift tail pointer
	} else {
		f->tail += f->nmemb; // shift tail pointer
	}
		
	memcpy(f->tail, val_ptr, f->nmemb);
	f->num_ele_buff += 1;
}

int return_index_of_least(funnel *f)
{
	// iterate over all subarrays and find the one that has the smallest dereferenced
	// value of head pointer
	int min_ind = -1;
	int i;
	for (i = 0; i < f->num_subarr; i++)
	{
		if (f->sub_funnels[i]->num_ele_buff > 0)
		{
			if (min_ind == -1)
				min_ind = i;
			if (f->cmp(f->sub_funnels[i]->head, f->sub_funnels[min_ind]->head) < 0)
				min_ind = i;
		}
	}
	return min_ind;
}

void fill_funnel(funnel *f)
{
	// keep filling the buffer until it's full
	while (f->num_ele_buff < (int)(L / f->nmemb) && f->num_ele_buff < f->stream_size)
	{
		int i;
		for (i = 0; i < f->num_subarr; i++)
		{
			if (f->sub_funnels[i]->num_subarr > 0 && f->sub_funnels[i]->num_ele_buff == 0 &&
				f->sub_funnels[i]->stream_size != 0)
			{
				fill_funnel(f->sub_funnels[i]);
			}
		}

		// after subbuffers are updated, there's at least one that is not empty
		// we iterate through all buffer heads and pick the one with the smallest value
		i = return_index_of_least(f);
		push_buffer(f, pop_buffer(f->sub_funnels[i]));
	}
}

void funnel_sort(void *arr, int size, size_t nmemb, cmp_t cmp)
{
	void *sorted = malloc(nmemb * size);
	int ptr = 0;
	funnel *f = create_funnel(arr, size, nmemb, cmp);

	do
	{
		fill_funnel(f);
		while (f->num_ele_buff > 0)
		{
			memcpy(sorted + nmemb * ptr++, pop_buffer(f), nmemb);
		}
	} while (f->stream_size > 0);

	memcpy(arr, sorted, size * nmemb);
}
