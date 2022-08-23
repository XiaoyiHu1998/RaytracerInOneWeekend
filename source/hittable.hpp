#ifndef HITTABLE_HPP
#define HITTABLE_HPP

#include "ray.hpp"
#include "axisAlignedBoundingBox.hpp"

class material; //tells compiler material class will be declared somewhere later on

struct hitRecord{
    point3 hitLocation;
    glm::vec3 normal;
    float distance;
    bool frontFace;

    float u;
    float v;
    
    std::shared_ptr<material> materialPointer;

    inline void setFaceNormal(const ray& r, const glm::vec3& outwardNormal){
        frontFace = glm::dot(r.direction(), outwardNormal) < 0.0;
        normal = frontFace ? outwardNormal : -outwardNormal;
    }
};

class hittable{
public:
    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const = 0;
    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const = 0;
};

#endif //HITTABLE_HPP