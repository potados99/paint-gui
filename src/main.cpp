#include <iostream>

#include "input.h"

int main(int argc, const char * argv[]) {
	std::cout << "hello world!" << std::endl;

	input imgr("/dev/input/event1");

	imgr.start_loop();

	return 0;
}
