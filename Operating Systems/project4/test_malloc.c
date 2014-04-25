#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

void test_malloc_free();
void test_calloc();
void test_realloc();

int main()
{
	int pid;

	pid = fork();
	if(pid == 0) {
		test_malloc_free();
		return 0;
	} else {
		wait(pid);
	}

	pid = fork();
	if(pid == 0) {
		test_calloc();
		return 0;
	} else {
		wait(pid);
	}

	pid = fork();
	if(pid == 0) {
		test_realloc();
		return 0;
	} else {
		wait(pid);
	}

	return 0;
}

void test_malloc_free()
{	
	printf("\n\e[1m[MALLOC TEST]\e[m\n");
	printf("a = malloc(int)\nb = malloc(int*5)\n");
	int *a = malloc(sizeof(int));
	int *b = malloc(sizeof(int)*5);
	heapchk();
	printf("free(a)\n");
	free(a);

	heapchk();

	/*Now test what happens when a block is "abandoned"
	  We do this by calling malloc, and then clearing the block's header
	*/
	printf("Now leaking an int[10] block:\n");
	bzero((void*)(malloc(sizeof(int)*10)-8), 8);

	heapchk();
	printf("free(b)");
	free(b);
	heapchk();

}

void test_calloc()
{
	printf("\n\e[1m[CALLOC TEST]\e[m\n");
	int i;
	int original_a;
	int* a = malloc(sizeof(int)*10);
	int* b = malloc(sizeof(int)*10); // make sure callc doesnt allocate the "extra" space

	original_a = (int)a; //store the original address

	printf("malloc(sizeof(int)*10) returned address \e[34m%p\e[m\n", a);
	for(i = 0; i < 10; i++) a[i] = i*10; //set the array to multiples of 10
	for(i = 0; i < 10; i++) printf("a[%d]=%d\n", i, a[i]); //print array
	printf("free(a)\n");
	free(a); //free the array

	/*
	re-accolcate the array of the same size. malloc() should convert 
	the block we just freed, and it should have our multiples of 10 in it.
	Since we called calloc, we expect that all values get zeroed out.
	*/
	a = calloc(sizeof(int), 10);
	printf("calloc(sizeof(int), 10) returned address \e[34m%p\e[m\n", a);
	for(i = 0; i < 10; i++) printf("[%d]=%d\n", i, a[i]);

	// Check that all values were actually zeroed 
	int worked = 1;
	for(i = 0; i < 10; i++) {
		if(a[i] != 0){
			worked = 0;
			break;
		}
	}

	if(!worked){
		printf("\e[31mcalloc() did not work as expected.\e[m\n");
	} else {
		printf("\e[32mcalloc() worked!(%s, zeroed)\n\e[m", ((original_a == (int)a)?"Same address":"new address"));
	}

	free(a);
	free(b);


}

void test_realloc()
{
	printf("\n\e[1m[REALLOC TEST]\e[m\n");
	printf("malloc() 20 integers:\n");
	int* arr = malloc(sizeof(int)*20);
	heapchk();

	printf("realloc() to 30 integers:\n");
	arr = realloc(arr, sizeof(int)*30);
	heapchk();


	printf("realloc() to 15 integers:\n");
	arr = realloc(arr, sizeof(int)*15);
	heapchk();

	free(arr);
	heapchk();
}