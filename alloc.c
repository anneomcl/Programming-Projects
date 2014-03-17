/** @file alloc.c */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define log(n) 32 - __builtin_clz(n)

typedef struct _metadata {
	size_t size;
	struct _metadata * next;
} metadata;

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
	if(size == 0)
		return sbrk(0);

	int temp = (int) size >> 10;
	int i = (int) log(temp);
	metadata * meta_ptr = seg_list[i];

	while(meta_ptr)
	{
		if(meta_ptr->size >= size)
		{
			seg_list[i] = meta_ptr->next;
			meta_ptr->next = NULL;
			return meta_ptr+1;
		}
		meta_ptr = meta_ptr->next;
	}
	meta_ptr = sbrk(size + sizeof(metadata));
	meta_ptr->size = size;
	meta_ptr->next = NULL;

	return (meta_ptr + 1);
}


/**
 * Deallocate space in memory
 */
void free(void *ptr) {
    if (!ptr)
        return;

	metadata * new = (metadata *) ptr;
	new = new - 1;

	int temp = (int) new->size >> 10;
	int i = (int) log(temp); 
	metadata * p = seg_list[i]; 
	new->next = p;
	seg_list[i] = new;


    return;
}


/**
 * Reallocate memory block
 */
void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }
    if (!size) {
        free(ptr);
        return NULL;
    }
	void * p = NULL;
	metadata * old_p = (metadata *) ptr;
	old_p = old_p - 1;
	if(old_p->size >= size)
		return ptr;
	p = malloc(size);
	p = memcpy(p, ptr, old_p->size);
	free(ptr);

    return p;
}

