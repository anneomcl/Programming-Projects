/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#define log(n) 32 - __builtin_clz(n)

//metadata contains the size of the memory block and a next pointer
typedef struct _metadata {
	size_t size;
	struct _metadata * next;
} metadata;

//buckets to store blocks by powers of 2
metadata * seg_list[25] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
//1, 2-3, 4-7, 8-15, 16-31, 32-63, 64-127, ....??
//0, 1  , 2  ,  3  ,   4  ,   5  ,   6   , ... 31

/**
 * Allocate space for array in memory
 */
void *calloc(size_t num, size_t size)
{
	void *ptr = malloc(num * size);
	
	if (ptr)
		memset(ptr, 0x00, num * size);

	return ptr;
}


/**
 * Allocate memory block
 */

void *malloc(size_t size) {  
	if(size == 0) //if block is 0, allocate no memory
		return sbrk(0);

	int temp = (int) size >> 10; //calculate which bucket the block should go into
	int i = (int) log(temp);
	metadata * meta_ptr = seg_list[i]; //set meta_ptr to beginning of appropriate bucket

	while(meta_ptr)
	{
		if(meta_ptr->size >= size) //find a free block that is large enough
		{
			seg_list[i] = meta_ptr->next;
			meta_ptr->next = NULL;
			return meta_ptr+1;
		}
		meta_ptr = meta_ptr->next; //otherwise, keep cycling through the list
	}
	meta_ptr = sbrk(size + sizeof(metadata)); //if no free block is found, sbrk to allocate new memory
	meta_ptr->size = size;
	meta_ptr->next = NULL;

	return (meta_ptr + 1); //meta_ptr + 1 so that the return value is the memory block itself
}


/**
 * Deallocate space in memory
 */
void free(void *ptr) {
    if (!ptr)
        return;

	metadata * new = (metadata *) ptr;
	new = new - 1; //access block

	int temp = (int) new->size >> 10; //calculate which bucket ptr should be in
	int i = (int) log(temp); 
	metadata * p = seg_list[i]; 
	new->next = p;
	seg_list[i] = new; //add ptr to bucket


    return;
}


/**
 * Reallocate memory block
 */
void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        return malloc(size); //if no valid destination pointer, simply malloc the block
    }
    if (!size) {
        free(ptr); 
        return NULL;
    }
	void * p = NULL;
	metadata * old_p = (metadata *) ptr; 
	old_p = old_p - 1; //access block
	if(old_p->size >= size) //if requested size is less than or equal to current size, do nothing
		return ptr;
	p = malloc(size); //if requested size is larger than the current size, and there is a valid destination ptr
	p = memcpy(p, ptr, old_p->size);// then malloc the memory and copy it
	free(ptr);

    return p;
}

