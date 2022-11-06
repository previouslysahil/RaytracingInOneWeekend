#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

struct hit_record;

class material {
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const = 0;
};

class lambertian: public material {
    public:
        lambertian(const color& a): albedo(a) {}

        virtual bool scatter(const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
            // Our hit record gave us the normal of the hittable that
            // our ray intersected with so we find a random point s 
            // in the normals unit sphere, this record point is given
            // by target = rec.p + rec.normal + random unit vector
            // Then we generate a direction from our normal origin in the random
            // direction created using our random target point this is
            // S - P or target - rec.p so our scatter direction simplifies
            // to this below
            auto scatter_direction = rec.normal + random_unit_vector();
            // Catch bad scatters near zero that could cause NaNs
            if (scatter_direction.near_zero()) {
                scatter_direction = rec.normal;
            }
            // Here we actually create the ray with its origin and direction
            scattered = ray(rec.p, scatter_direction);
            // Our color
            attenuation = albedo;
            return true;
        }
    
    public:
        color albedo;
};

class metal: public material {
    public:
        metal(const color& a): albedo(a) {}
    
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            // For our metal materials we need to get the reflected vector
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            // Here we actually create the ray with its origin and direction
            scattered = ray(rec.p, reflected);
            // Our color
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
    
    public:
        color albedo;
};

#endif
