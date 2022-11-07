#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>

color ray_color(const ray &r, const hittable& world, int depth) {
    // Information of where our ray hit our object
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) {
        return color(0, 0, 0);
    }
    // Go through our worlds object list
    // and check what we hit
    // Our hit record is set for our ray r which gives us the point
    // our ray hit and the normal from that point
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            // This recursive ray color calculation keeps going until our
            // ray stops hitting a and object in our world, falling
            // past our if statement, attenuation is our color which (always)
            // being less than 1 for x, y, z will slowly decrease our color
            // on each recursive call acting like shading
            // Since this is recursive eventually the ray will fall through
            // not hitting any object leading the color to be decided by
            // the code below which makes our sky, this is what shades
            // our world object with sky color UNLESS we exceed our depth 
            // which leads to getting the black color which could lead to black 
            // spots this is probably what happens in blender in glass!!
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        // Happens in metal when normal and scattered direction
        // are not similar meaning the ray is bouncing inwards?
        // so make it black?
        return color(0, 0, 0);
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

// x is horizontal, y is vertical, z is depth
int main() {

    // Image dimensions
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.0),   0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, -1.0), -0.45, material_left));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -1.0),   0.5, material_right));

    // Camera
    // Decides our camera's orientation to the look at point
    // in it's local axes
    vec3 local_up_direction = vec3(0, 1, 0);
    // Where our camera is positioned in our scene
    point3 lookfrom = point3(-2, 2, 1);
    // Where we are looking at in our scene
    point3 lookat = point3(0, 0, -1);
    // Make our camera with fov 90 and aspect ratio
    camera cam(lookfrom, lookat, local_up_direction, 20, aspect_ratio);

    // Start of PPM format requires P3 for color space
    // and image width and height
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        // Print out to err to see progress
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++) {
                // random double 'swerves' u and v into neighboring pixel
                // making our ray calculation blend surrounding pixels
                // calculated ray colors
                // u akin to moving along x values of image towards the right
                auto u = double(i + random_double()) / (image_width - 1);
                // v akin to moving along y values of image towards the bottom
                auto v = double(j + random_double()) / (image_height - 1);
                // Create our ray from our cameras origin using our images pixels
                // to form our direction which we get from u and v
                ray r = cam.get_ray(u, v);
                // Adds to color by using unit direction and y coordinate to
                // generate gradient between white and blue
                // Also generates the shading for all of our hittable
                // objects using normal shading
                pixel_color += ray_color(r, world, max_depth);
            }
            // Formats for ppm and averages our samples
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}
