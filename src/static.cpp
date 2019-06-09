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
color color::black(0, 0, 0);
color color::white(255, 255, 255);
color color::red(255, 0, 0);
color color::green(0, 255, 0);
color color::blue(0, 0, 255);
color color::yellow(255, 255, 0);
color color::cyan(0, 255, 255);
color color::maganta(255, 0, 255);

