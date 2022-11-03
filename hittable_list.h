#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list: public hittable {
    public:
        hittable_list() {}
        hittable_list(shared_ptr<hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<hittable> object) { objects.push_back(object); }

        virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

    public:
        std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    auto closes_so_far = t_max;

    // Look through all of our stored objects
    // finding min object to our ray
    for (const auto& object: objects) {
        // If the object is hittable by our ray assign its record
        // until we find a closer object, with our
        // closes_so_far
        // This is basically just finding the min object in objects
        if (object->hit(r, t_min, closes_so_far, temp_rec)) {
            hit_anything = true;
            closes_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

#endif