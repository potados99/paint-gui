#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "display.h"
#include "machine_specific.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
	printf("back to c\n");
    
    int             dp_fd;
    unsigned short  *dp_mem;
    
    dp_fd = open(DP_FD_PATH, DP_OPEN_OPTION);
    ASSERTDO(dp_fd != -1, print_error("open() error with %s.\n", DP_FD_PATH); return -1);
    
    dp_mem = disp_map(dp_fd);
    
    
	return 0;
}

