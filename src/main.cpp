#include <iostream>
#include <thread>
#include "touch.h"
#include "shape.h"
#include "visual_element.h"
#include "app.h"
#include "display.h"

int main(int argc, const char * argv[]) {
	std::cout << "hello world!" << std::endl;
    
    visual_element ve1(point(0, 0), size(0, 0), shape(shape::type::RECT, color(0,0,0)));
    
    std::cout << ve1.to_string() << std::endl;

    app myapp("/dev/input/event1");
    
    
    
    
    
	return 0;
}
