#include <unistd.h>
#include <stdio.h>
#include <strings.h>

#include "malloc.h"


void* malloc(size_t size)
{
	size_t size8 = align8(size); // the 8-byte aligned size of the data
	size_t alloc_size = size8+H_BLOCK; // full size required for block
	
	block alloc = NULL;
	
	if(malloc_head == NULL){ //no blocks in the free list

		if(heap_start == NULL){// get the starting address of the heap
			heap_start = sbrk(0);
		}
		alloc = alloc_heap(alloc_size); // extend heap to allocate space

	} else { // try to find a block to use
		free_block cur;

		for_each_free(cur){

			if(cur->size >= alloc_size){ // large enough to carve out a block_t
				int end = (int)cur + cur->size + H_FREE;
				alloc = (block)(end-alloc_size);
				printf("%d\n", (int)alloc);
				cur->size = cur->size-alloc_size; // adjust size of source block
				break;
			} else if (cur->size + H_FREE == alloc_size) { // correct size to convert to a block_t
				remove_from_free(cur);	
				alloc = (block)cur;	
				break;
			}
		}	
								
		if(alloc == NULL) { // no large enough free block was found
			alloc = alloc_heap(alloc_size); // extend heap to allocate space
		}
	}

	alloc->size = size8;
	set_checksum(alloc);

	return (void*)(((int)alloc+H_BLOCK));	
}

/*
	Frees the pointer by adding the data block to the free list
*/
void free(void* ptr)
{
	block b = (block)(ptr-H_BLOCK);

	if(ptr == NULL || !validate_checksum(b)) // check if the pointer is freeable
		return;
		
	//printf("\n[---------[    FREE    ]---------]\n");
	
	b->checksum = 0; // remove checksum

	free_block f = (free_block)b; //"convert" to a free_block
	f->size = f->size-4;

	add_to_free(f);
}

void* calloc(size_t num, size_t size)
{
	void* ptr = malloc(num*size);
	bzero(ptr, num*size);
	return ptr;

}

void* realloc(void* ptr, size_t size)
{
	void* newptr;
	block b = ((block)((int)ptr-H_BLOCK));
	int inc = size - (b->size);

	if(inc <= -((int)H_FREE)){ // if lowering size by enough, "carve" a free_block out of the block
		int fb_addr = (((int)b) + H_BLOCK + (b->size) + inc); //get the address of the new free_block
		free_block fb = (free_block)fb_addr;
		fb->size = (-inc)-((int)H_FREE); 
		add_to_free(fb); // add to free list

		b->size = size;  // set new size of block
		set_checksum(b); //set checksum of block
		newptr = ptr;	// the new pointer is the same as the old one
	} else {
		newptr = malloc(size); // allocate a new block
		int copy_size = (((b->size) < size)?b->size:size);
		bcopy(ptr, newptr, copy_size); // copy contents to new block
		free(ptr); //free old block
	}
	return newptr;
}

/*
	Extends the heap by inc*2 bytes, returns a pointer to a block with size inc bytes
*/
void* alloc_heap(size_t inc)
{

	void* new_buffer = sbrk(inc*2); // allocate twice the requested amount so there is some extra	
	free_block spare = (free_block)(new_buffer + inc); // calculate address of extra space

	spare->size = inc-H_FREE; // add the spare to the free list
	add_to_free(spare);

	heap_end = sbrk(0); //update end of heap

	return new_buffer;
}



void heapchk()
{
	printf("\n[-------------[HEAP CHECKER]-------------]\n");

	printf("[Heap]\t(%d, %d):%d bytes\n", (int)heap_start, (int)heap_end, (int)(heap_end-heap_start));
	char* ptr = heap_start;
	block cur_used;
	free_block cur_free;
	heapchk_found found;
	int unmanaged_start = 0;
	int unmanaged_end = 0;

	ptr = heap_start;

	while((int)ptr < (int)heap_end){
		cur_used = (block)ptr;
		found = NONE;

		if(validate_checksum(cur_used)){
			found = BLOCK;
		} else {
			for_each_free(cur_free){
				if((int)ptr == (int)cur_free){
					found = FREE;

					break;
				}
			}
		}

		if(found == NONE){
			if(unmanaged_start == 0){
				unmanaged_start = (int)ptr;
			}
			unmanaged_end = (int)ptr;
			ptr++;

		} else {

			printf("------------------------\n");
			
			if(unmanaged_start != 0){

				printf("\e[31m%d bytes UNMANAGED @ %d\e[m\n", (unmanaged_end-unmanaged_start)+1, unmanaged_start);
				unmanaged_start = 0;
				unmanaged_end = 0;
				//*((int*)(heap_end+1)) = 3; // cause a segfault

				printf("------------------------\n");
			}

			if(found == BLOCK){

				printf("\e[36m%d bytes BLOCK @ %d\e[m\n", (cur_used->size)+H_BLOCK, (int)ptr);
				ptr += (cur_used->size) + H_BLOCK;

			} else if (found == FREE) {

				printf("\e[32m%d bytes FREE @ %d\e[m\n", (cur_free->size)+H_FREE, (int)ptr);
				ptr += (cur_free->size) + H_FREE;

			}
		}
	}
	
	printf("------------------------\n");

	freechk();
}
 
void freechk()
{
	printf("[Free blocks]\n");
	free_block fb;
	for_each_free(fb){
		printf("   [%d]={%d, %d, %d}  (%d, %d)\n", (int)fb, fb->size, (int)fb->next, (int)fb->prev, (int)fb, (int)fb+fb->size+H_FREE);
	}

	// test if free list can be traversed both ways
	if((fb = malloc_head) != NULL){
		while(fb->next != NULL)fb = fb->next;
		while(fb->prev != NULL)fb = fb->prev;
		printf("   %s\n", (fb == malloc_head)?"List is correctly linked":"BAD LINKAGES");
	}

	printf("   malloc_head=%d", (int)malloc_head);
	printf("\n\n");

}

/*
	Sets the checksum for a data block
	The algorithm im simply the address of the block XOR the size of the block
*/
void set_checksum(block b){
	
	b->checksum = ((int)(&malloc))-((int)b); //&block XOR block->size
}

/*
	returns whether the checksum of the block is valid as a block
*/
int validate_checksum(block b){
	return (b->checksum == ((int)(&malloc))-((int)b));
}

/*
	Removes fre_block f from the free_list. 
	The removed block should be added back to the free_list or allocated to avoid fragmentation
*/
void remove_from_free(free_block f){
	if(f->prev != NULL)f->prev->next = f->next; // connect f's prev and next blocks together
	if(f->next != NULL)f->next->prev = f->prev;
	if(f == malloc_head) malloc_head = f->next; // set malloc_head to the next free_block if needed
	f->prev = NULL;
	f->next = NULL; 
}

/*
	free_block f is added to the free list, and then merge_free_block() is called
*/
void add_to_free(free_block f){
	free_block last = malloc_head;

	if(malloc_head == NULL){
		malloc_head = f;
		f->prev = NULL;
	} else {
		while( last->next != NULL)last = last->next; // find the last block in the free list

		last->next = f;
		f->prev = last;
	}

	f->next = NULL;	

	merge_free_block(f); // try to coalesce blocks
}

/*
	Merges free_block f with any free blocks that may be around it
*/
void merge_free_block(free_block f){

	free_block cur;
	int merged = 0;

	//freechk(); 

	for_each_free(cur){ // check for a block adjacent before
		if((int)cur+cur->size+H_FREE == (int)f){
			//f is guaranteed to be at the end of the list when this is called
			if(f->prev != NULL)f->prev->next = NULL; // remove f from the list
			cur->size += f->size + H_FREE; // new size of cur
			bzero(f, H_FREE); //erase f's free_block metadata
			f = cur;
			break;
		}
	}

	for_each_free(cur){ //check for a block adjacent after
		if((int)f+f->size+H_FREE == (int)cur){
			// cur may be anywhere, so remove it, and merge with f
			//remove cur from the free list first
			remove_from_free(cur);
			f->size += cur->size+H_FREE;
			bzero(cur, H_FREE);
			break;
		}
	}

	//freechk();
}


