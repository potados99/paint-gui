#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <sys/time.h>

static unsigned short static_arr[76800];

int main(int argc, const char * argv[]) {
	unsigned short stack_arr[76800];
	unsigned short *heap_arr;

	heap_arr = (unsigned short *)malloc(sizeof(unsigned short) * 76800 + 1);

rw_test_area: {
	int test_repeat = 200;
	int rw_repeat = 1000;

	int elapsed = 0;

	unsigned long long int static_total = 0;
	unsigned long long int stack_total = 0;
	unsigned long long int heap_total = 0;
	
	for (int i = 1; i <= test_repeat; ++i) {
		printf("================== TEST %d BEGIN ===================\n", i);
		
		printf("Whole read and write for %d times.\n", rw_repeat);

		elapsed = rw_test(static_arr, rw_repeat);
		printf("static_arr: %dms elapsed\n", elapsed);
		static_total += elapsed;

		elapsed = rw_test(stack_arr, rw_repeat);
		printf("stack_arr: %dms elapsed\n", elapsed);
		stack_total += elapsed;

		elapsed = rw_test(heap_arr, rw_repeat);
		printf("heap_arr: %dms elapsed\n", elapsed);
		heap_total += elapsed;
		
		printf("================== TEST %d DONE ===================\n\n", i);

	} 

	puts("================== RESULT ===================");
	printf("Average time for %d times whole read/write:\n\n", rw_repeat);	

	printf("static_arr: %lfms.\n", (double)static_total / test_repeat);
	printf("stack_arr: %lfms.\n", (double)stack_total / test_repeat);
	printf("heap_arr: %lfms.\n", (double)heap_total / test_repeat);
	puts("=============================================");

	return 0;
}

list_test_area:
	return 0;
}

int rw_test(unsigned short *arr, int repeat) {
	/**
	  * Wherever the array is...
	  */

	struct timeval start;
	struct timeval end;
	struct timeval elapsed;

	gettimeofday(&start, NULL);

	for (register int r = 0; r < repeat; ++r) {
		for (register int i = 0; i < 76800; ++i) {
			*(arr + i) += 1; /* read and write. */
		}
	}

	gettimeofday(&end, NULL);

	timersub(&end, &start, &elapsed);

	return ((elapsed.tv_sec * 1000) + (elapsed.tv_usec / 1000));
}


