#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere: public hittable {
    public:
        sphere() {}
        sphere(point3 cen, double r): center(cen), radius(r) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        point3 center;
        double radius;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    // This math solves for t in this equation:
    // (A + tb - C) * (A + tb - C) = r^2 (equation of sphere)
    // A (origin), b (direction), C (center of sphere) are vectors
    // t is a real number
    // Essentially checking if our ray from our origin has
    // hit the sphere on its way to the viewport plane
    // Remember t is the distance along our ray from
    // the origin to the destination
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;
    // No real roots meaning no solution
    if (discriminant < 0) return false;
    // Now we look at both of our roots (if we have 2
    // and take the closest one)
    auto sqrtd = sqrt(discriminant);
    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        // Look if 'positive' root is in acceptable range
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            // root aka 't' not in our min max range
            return false;
        }
    }

    // Sovled for our t above and chose closest root
    rec.t = root;
    // The t-th point along our ray in A + tb
    // AKA point our ray intersects this sphere
    rec.p = r.at(rec.t);
    // Now we get our surface normal which is
    // the vector perpindicular to the surface
    // at point of intersection
    // This is P - C (where C is center and P
    // is the surface point)
    // We normalize to the unit vector for shading
    vec3 outward_normal = (rec.p - center) / radius;
    // Sets rec.front_face and rec.normal
    rec.set_face_normal(r, outward_normal);

    return true;
}

#endif