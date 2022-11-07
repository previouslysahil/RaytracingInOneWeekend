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

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
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
        metal(const color& a, double f): albedo(a), fuzz(f < 1 ? f : 1) {}
    
        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            // For our metal materials we need to get the reflected vector
            vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
            // Here we actually create the ray with its origin and direction
            // here fuzziness double randomizes the reflected direction creates
            // a 'fuzzy' effect on the reflection of our metal surface.
            scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
            // Our color
            attenuation = albedo;
            return (dot(scattered.direction(), rec.normal) > 0);
        }
    
    public:
        color albedo;
        double fuzz;
};

class dielectric: public material {
    public:
        dielectric(double index_of_refraction): ir(index_of_refraction) {}

        virtual bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const override {
            // Glass is white
            attenuation = color(1.0, 1.0, 1.0);
            // Our refraction amount is dependent on our normal directions
            double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
            // Necessary for snells law calculation below
            vec3 unit_direction = unit_vector(r_in.direction());

            // Voodoo magic to see if we should refract this ray
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction;
            // Reflect or refract using schlick approx for second OR
            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
                // Reflect if we cannot refract
                direction = reflect(unit_direction, rec.normal);
            } else {
                // Does some weird math im too lazy to understand right now to make 
                // refracted ray direction
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            }
            // Make our ray
            scattered = ray(rec.p, direction);
            return true;
        }
    
    public:
        double ir;

    private:
        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0;
            return r0 + (1 - r0) * pow((1 - cosine), 5);
        }
};

#endif
