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
color color::black = color(0, 0, 0);
color color::white = color(255, 255, 255);
color color::red = color(255, 0, 0);
color color::green = color(0, 255, 0);
color color::blue = color(0, 0, 255);
color color::yellow = color(255, 255, 0);
color color::cyan = color(0, 255, 255);
color color::maganta = color(255, 0, 255);

