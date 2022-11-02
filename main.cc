#include "vec3.h"
#include "color.h"

#include <iostream>

int main() {

    // Image dimensions
    const int image_width = 256;
    const int image_height = 256;

    // Start of PPM format requires P3 for color space
    // and image width and height
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        // Print out to err to see progress
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            // Red works from low to high values horizontally
            double r = double(i) / (image_width-1);
            // Green works from high to low value vertically
            double g = double(j) / (image_height-1);
            // Static blue
            double b = 0.25;
            // Color is actually just a vec3
            color pixel_color(r, g, b);
            // Formats for ppm
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}
