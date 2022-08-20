#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include "ray.hpp"
#include "axisAlignedBoundingBox.hpp"

class material; //tells compiler material class will be declared somewhere later on

struct hitRecord{
    point3 hitLocation;
    vec3 normal;
    double distance;
    bool frontFace;

    double u;
    double v;
    
    std::shared_ptr<material> materialPointer;

    inline void setFaceNormal(const ray& r, const vec3& outwardNormal){
        frontFace = dot(r.direction(), outwardNormal) < 0.0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class hittable{
public:
    virtual bool hit(const ray& r, double distMin, double distMax, hitRecord& record) const = 0;
    virtual bool boundingBox(double tStart, double tEnd, axisAlignedBoundingBox& refbox) const = 0;
};

#endif //HITTABLE_HPP