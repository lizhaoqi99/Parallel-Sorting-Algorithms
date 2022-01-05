typedef int (*cmp_t)(const void *, const void *);

typedef struct funnel {
	struct funnel **sub_funnels;	// subfunnels
	void* begin_sort_ptr;		// the pointer to the start of the input array in which the funnel would sort
	int size;					// total number of elements in the funnel 
	int num_subarr;			    // number of subarrays after rounding from n^(1/3)
	int stream_size;			// the size of the remaining items in the buffer stream
	size_t nmemb; 	            // data type size
	cmp_t cmp;		            // compares to 
	void *buff;					// output buffer
	int num_ele_buff;	        // number of elements in the buffer
	void *head;                 // head of the funnel
	void *tail;                 // tail of the funnel
} funnel;

int comparator(const void *a, const void *b);

funnel * create_funnel(void* arr, int size, size_t nmemb, cmp_t cmp);

void * pop_buffer(funnel* f);

void push_buffer(funnel* f, void* value_p);

int return_index_of_least(funnel *f);

void fill_funnel(funnel *f);


void funnel_sort(void *arr, int nmemb, size_t size, cmp_t cmp);