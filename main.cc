#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

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
        // Our hit record gave us the normal of the hittable that
        // our ray intersected with so we find a random point s 
        // in the normals unit sphere
        point3 target = rec.p + rec.normal + random_unit_vector();
        // Then we generate a ray from our normal origin in the random
        // direction created using our random target point
        // This recursive ray color calculation keeps going until our
        // ray stops hitting a and object in our world, falling
        // past our if statement, use 0.5 to decrease the strength
        // Since this is recursive eventually the ray will fall through
        // not hitting any object leading the color to be decided by
        // the code below which makes our sky, this is what shades
        // our world spheres UNLESS we exceed our depth which leads
        // to getting the black color which could lead to black spots
        // this is probably what happens in blender in glass!!
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
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
    // Our OG sphere
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    // Bascially placing sphere basee right under this 
    // sphere and making it huge
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // Camera
    camera cam;

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
