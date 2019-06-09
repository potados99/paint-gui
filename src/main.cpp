#include <iostream>
#include <thread>
#include "touch.h"
#include "shape.h"
#include "visual_element.h"
#include "app.h"
#include "display.h"
#include "machine_specific.h"
#include "color.h"
#include "metric.h"

int main(int argc, const char * argv[]) {
	std::cout << "hello world!" << std::endl;
    
    /*
    visual_element ve1(point(0, 0), size(0, 0), shape(shape::type::RECT, color(0,0,0)));
    
    std::cout << ve1.to_string() << std::endl;

    app myapp("/dev/input/event1");
    
    */
    
    display mydisp(DPPATH);
    
    mydisp.clear();

    std::cout << "cleared" << std::endl;

    usleep(1000000);
    
    mydisp.clear(color16::blue);
    mydisp.commit();

    std::cout << "blued" << std::endl;
    
    usleep(1000000);

    mydisp.draw_rect(point(0, 0), size(200, 200), color16::white);
    mydisp.commit();

    std::cout << "yeah" << std::endl;
 


	for (int i = 0; i < 119; ++i) {
		// usleep(1);
		mydisp.draw_rect(point(i, 0), size(200, 200), color16::blue);
		mydisp.draw_rect(point(i + 1, 0), size(200, 200), color16::white);
		mydisp.commit();
	}






   
	return 0;
}
