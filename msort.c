/** @file msort.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//struct to contain inputs necessary for multi-threaded qsort
typedef struct qsort_inputs 
{
	int * list;
	int num_to_sort;
	
} qsort_inputs;

//struct to contain inputs necessary for multi-threaded merge
typedef struct merge_inputs 
{
	int * list1;
	int size1;
	int * list2;
	int size2;
	int new_size;
} merge_inputs;

//compare function necessary for qsort
int cmpfunc (const void * a, const void * b)
{
	return ( *(int*)a - *(int*)b );
}

//qsort function that returns a void ptr 
void * void_qsort(void * arg)
{ 
	struct qsort_inputs* x = (struct qsort_inputs *) arg; 
	qsort(x->list, x->num_to_sort, sizeof(int), cmpfunc);
	fprintf(stderr, "Sorted %d elements.\n", x->num_to_sort); 
	return x->list; 
}

//merge function that returns a void ptr
void * void_merge(void * arg)
{
	struct merge_inputs* x = (struct merge_inputs *) arg;
	int i = 0;
	int j = 0;
	int k = 0;
	int dupes = 0;

	int* temp_list1 = malloc(sizeof(int) * x->size1);
	for(i = 0; i < x->size1; i++)
		temp_list1[i] = x->list1[i];

	for(i = 0; i < x->new_size;) //merging all values into list1, noting duplicate values in dupes
	{
		if(j < x->size1 && k < x->size2)
		{
			if(temp_list1[j] < x->list2[k])
			{
				x->list1[i] = temp_list1[j];
				j++;
			}

			else if(x->list2[k] < temp_list1[j])
			{
				x->list1[i] = x->list2[k];
				k++;
			}
	
			else if(temp_list1[j] == x->list2[k])
			{
				x->list1[i] = temp_list1[j];
				j++;
				dupes++;
			}

			i++;
		}

		else if(j == x->size1)
		{
			for(; i < x->new_size;)
			{
				x->list1[i] = x->list2[k];
				k++;
				i++;
			}
		}

		else
		{
			for(; i < x->new_size;)
			{
				x->list1[i] = temp_list1[j];
				j++;
				i++;
			}
		}
	}
	free(temp_list1);
	fprintf(stderr, "Merged %d and %d elements with %d duplicates.\n", x->size1, x->size2, dupes);
	return x->list1;
}

int main(int argc, char **argv)
{

	if(argc != 2) //if invalid argument size
	{
		printf("ERROR\n"); 
		return 0;
	}

	//initial capacity set to 50, segment count determined from args
	int capacity = 50; 
	int input_count = 0; 
	int * input_list = malloc(sizeof(int) * 50); 
	int segment_count = atoi(argv[1]); 

	char input[12]; //assuming number will not be greater than 12 digits

	while(fgets(input, 12, stdin) != NULL) //
	{
		if(input_count == capacity) //resize if there are more than 50 inputs
		{
			input_list = realloc(input_list, sizeof(int) * capacity * 2);
			capacity = capacity * 2;
		}

		input_list[input_count++] = atoi(input); //increment input_count 
						     //and add an int-ify'd number to inputs
	}

	int values_per_segment; //determine values per segment
	int remainder = 0;

	if (input_count % segment_count == 0) //determine number of segments
	{
    		values_per_segment = input_count / segment_count;
		remainder = values_per_segment;
	}

	else //if segments don't divide evenly, set a remainder
	{
    		values_per_segment = (input_count / segment_count) + 1;
		remainder = input_count - (values_per_segment * (segment_count - 1));
	}

	struct qsort_inputs* qin_list = malloc(sizeof(struct qsort_inputs) * segment_count); //list of "n" structs for qsort input
	
	int i;
	for(i = 0; i < segment_count; i++) //initialize list of structs
	{	
		int index = i * values_per_segment;
		qin_list[i].list = input_list + index;
		qin_list[i].num_to_sort = values_per_segment;
	}
 
	qin_list[segment_count - 1].num_to_sort = remainder; //set the remainder

	pthread_t thread_name[segment_count]; //initialize array of threads

	for(i = 0; i < segment_count; i++)
	{ 
		pthread_create(&thread_name[i], NULL, void_qsort, &qin_list[i]); //perform multi-threaded qsort
	}

	for(i = 0; i <segment_count; i++)
	{ 
		pthread_join(thread_name[i], NULL); //join all threads
	}

	while(segment_count > 1) //while there are still multiple lists to be merged
	{	 	
		int merge_count = segment_count/2; //find number of times to execute merge
		struct merge_inputs* merge_list = malloc(sizeof(merge_inputs) * merge_count); //array of structs for merge inputs

		for(i = 0; i < segment_count - 1 ; i += 2) //set all merge structs to their proper values
		{ 
			merge_list[i/2].list1 = qin_list[i].list;
			merge_list[i/2].list2 = qin_list[i+1].list;
			merge_list[i/2].size1 = qin_list[i].num_to_sort;
			merge_list[i/2].size2 = qin_list[i+1].num_to_sort;
			merge_list[i/2].new_size = qin_list[i].num_to_sort + qin_list[i+1].num_to_sort;
		} 
	
		pthread_t thread_name[merge_count]; //initialize array of threads

		for(i = 0; i < merge_count; i++)
		{ 
			pthread_create(&thread_name[i], NULL, void_merge, &merge_list[i]); //perform multi-threaded merge
		} 
	
		for(i = 0; i < merge_count;i++)
		{ 
			pthread_join(thread_name[i],NULL); //join threads
		} 
	
		int new_segment_size = segment_count - merge_count; //merge_count + segment_count%2
	
		for(i = 0; i < new_segment_size; i++)
		{ 
			if(segment_count%2 && i == new_segment_size - 1) //set new values if there is a remainder
			{
				qin_list[i].list = qin_list[segment_count - 1].list; 
				qin_list[i].num_to_sort = qin_list[segment_count - 1].num_to_sort; 
			} 
	
			else //set new values assuming all segments are even
			{
				qin_list[i].list = merge_list[i].list1; 
				qin_list[i].num_to_sort = merge_list[i].new_size;
			} 
		} 
	
		segment_count = new_segment_size; 
		free(merge_list);
	}	

	int a;
	for(a = 0; a < input_count; a++)
		printf("%d\n", qin_list[0].list[a]); //print all values

//freeing memory
	free(input_list);
	free(qin_list);
	return 0;
}
