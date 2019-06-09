#include "object.h"
#include "visual_element.h"
#include "color.h"

/**
 * All static member variables here.
 */

/**
 * class object
 */
unsigned long object::object_count = 0;

/**
 * class visual_element
 */
int visual_element::element_count = 0;

/**
 * class color
 */
color16 color16::black(0, 0, 0);
color16 color16::white(255, 255, 255);
color16 color16::red(255, 0, 0);
color16 color16::green(0, 255, 0);
color16 color16::blue(0, 0, 255);
color16 color16::yellow(255, 255, 0);
color16 color16::cyan(0, 255, 255);
color16 color16::maganta(255, 0, 255);

