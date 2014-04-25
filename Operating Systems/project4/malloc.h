#ifndef _MALLOC_
#define _MALLOC_

#include <unistd.h>

// convenience macro for iterating through the free list
#define for_each_free(fb) for((fb) = malloc_head; (fb) != NULL; (fb) = (fb)->next)

//rounds a number up to the next (or equal) multiple of 8
#define align8(b) (((((b)-1)>>3)+1)<<3)

#define H_BLOCK (sizeof(struct block_t))
#define H_FREE (sizeof(struct free_block_t))

typedef enum {NONE, BLOCK, FREE} heapchk_found;

typedef struct block_t *block;
typedef struct free_block_t *free_block;

typedef struct block_t {
	int size;
	int checksum;
} *block;

typedef struct free_block_t {
	int size;
	free_block next;
	free_block prev;
} *free_block;

free_block malloc_head = NULL;

void* heap_start = NULL;
void* heap_end = NULL;

void* malloc(size_t size);
void free(void* ptr);

void* calloc(size_t num, size_t size);
void* realloc(void* ptr, size_t size);

void* alloc_heap(size_t inc);

void heapchk();
void freechk();

void set_checksum(block b);
int validate_checksum(block b);

void remove_from_free(free_block f);
void add_to_free(free_block f);
void merge_free_block(free_block f);

#endif
