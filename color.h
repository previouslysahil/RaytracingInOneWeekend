#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

#include <iostream>

// Format of each pixel's rgb value in ppm
void write_color(std::ostream &out, color pixel_color) {
    // Write the translated [0,255] value of each color component
    // Casting clamps down so using 255.99 preserves from 0-1
    // most values when mapping
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

#endif