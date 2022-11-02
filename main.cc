#include "vec3.h"
#include "ray.h"
#include "color.h"

#include <iostream>

bool hit_sphere(const point3& center, double radius, const ray& r) {
    // This math solves for t in this equation:
    // (A + tb - C) * (A + tb - C) = r^2
    // A (origin), b (direction), C (center of sphere) are vectors
    //  t is a real number
    // Essentially checking if our ray from our origin has
    // hit the sphere on its way to the viewport plane
    // Remember t is the distance along our ray from
    // the origin to the destination
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}

color ray_color(const ray &r) {
    // Checking bounds of our sphere
    if (hit_sphere(point3(0, 0, -1), 0.5, r)) {
        return color(1, 0, 0);
    }
    // Unit vector is normalized direction
    vec3 unit_direction = unit_vector(r.direction());
    // Makes t go from 0 to 1 since y is between -1 & 1
    // since y is a unit vector (between -1 & 1)!!
    auto t = 0.5 * (unit_direction.y() + 1.0);
    // Interpolation simple 1.0 - t * v1 + t * v2
    // think dijkstras algorithm (bezier curves)
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main() {

    // Image dimensions
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // Camera
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0;

    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    // makes a vector for lower left with respect to origin combining x and y
    // from horizontal and vertical respectively,
    // focal length is used for z
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    // Start of PPM format requires P3 for color space
    // and image width and height
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        // Print out to err to see progress
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            // u akin to moving along x values of image towards the right
            auto u = double(i) / (image_width-1);
            // v akin to moving along y values of image towards the bottom
            auto v = double(j) / (image_height-1);
            // Generates the 'ray' for each pixel starting at left top,
            // moving right horizontally, and moving down vertically,
            // - origin for origin offset
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            // Makes color by using unit direction and y coordinate to
            // generate gradient between white and blue
            color pixel_color = ray_color(r);
            // Formats for ppm
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}
