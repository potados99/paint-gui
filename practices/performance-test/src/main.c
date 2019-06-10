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

	int repeat = 5000;
	printf("Whole read and write for %d times.\n", repeat);

	printf("static_arr: %dms elapsed\n", rw_test(static_arr, repeat));
	printf("stack_arr: %dms elapsed\n", rw_test(stack_arr, repeat));
	printf("heap_arr: %dms elapsed\n", rw_test(heap_arr, repeat));

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


