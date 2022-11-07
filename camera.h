#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

class camera {
    public:
        camera(
            point3 lookfrom,
            point3 lookat,
            vec3 vup, 
            double vfov, 
            double aspect_ratio,
            double aperture,
            double focus_dist
        ) {
            auto theta = degrees_to_radians(vfov);
            auto h = tan(theta / 2);
            auto viewport_height = 2.0 * h;
            auto viewport_width = aspect_ratio * viewport_height;
            
            // Create u, v, w which form orthonormal basis
            // of the 'plane' lookfrom point lives in when
            // pointing towars look at
            // u is like x, v is like y, and w is like z
            // w is vector from our camera from to view at
            w = unit_vector(lookfrom - lookat);
            // u is horizontal vector, vup is just an
            // arbitrary vector pointing 'up' which
            // basically dictates the 'up' direction of our
            // camera, since we cross product with w we
            // get our proposed u or local x axis of our camera
            u = unit_vector(cross(vup, w));
            // Cross product gives us vector perpendicular to
            // the plane spanned by w and u giving us v
            // which points up 'relatively' in our orthonormal
            // basis
            v = cross(w, u);

            origin = lookfrom;
            // Enlarges our camera view for our focus_dist
            horizontal = focus_dist * viewport_width * u;
            vertical = focus_dist * viewport_height * v;
            // makes a vector for lower left with respect to origin combining x and y
            // from horizontal and vertical respectively,
            // w is used for z
            lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;
            lens_radius = aperture / 2;
        }

        ray get_ray(double s, double t) const {
            // Lens depth of field
            vec3 rd = lens_radius * random_in_unit_disk();
            vec3 offset = u * rd.x() + v * rd.y();
            // Generates the 'ray' for each pixel starting at left top,
            // moving right horizontally, and moving down vertically,
            // - origin for origin offset
            // origin first, then direction of our pixel from our origin
            return ray(
                origin + offset,
                lower_left_corner + s * horizontal + t * vertical - origin - offset
            );
        }

    private:
        point3 origin;
        point3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u, v, w;
        double lens_radius;
};

#endif
