#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hittable.hpp"
#include "vec3.hpp"

class sphere : public hittable{
private:
    vec3 center;
    double radius;
    std::shared_ptr<material> materialPointer;
public:
    sphere(){}
    sphere(vec3 center, double radius, std::shared_ptr<material> materialPointer):
        center{center},
        radius{radius},
        materialPointer{materialPointer}
        {}

    virtual bool hit(const ray& r, double distMin, double distMax, hitRecord& record) const override;
};

bool sphere::hit(const ray& r, double distMin, double distMax, hitRecord& record) const {
    vec3 originMinCenter = r.origin() - center;
    vec3 rayDirection = r.direction();
    double a = rayDirection.lengthSquared();
    double bHalf = dot(rayDirection, originMinCenter);
    double c = originMinCenter.lengthSquared() - radius * radius;
    double discriminant = bHalf*bHalf - a*c;
    
    if(discriminant < 0.0)
        return false;

    double sqrtDiscriminant = std::sqrt(discriminant);
    double hitDistance = (-bHalf - sqrtDiscriminant) / a;
    //TODO: check if performance increases when you remove the extra branch
    if(hitDistance < distMin || hitDistance > distMax){
        hitDistance = (-bHalf + sqrtDiscriminant) / a;
        if(hitDistance < distMin || hitDistance > distMax)
            return false;
    }

    record.distance = hitDistance;
    record.hitLocation = r.at(hitDistance);
    vec3 outwardNormal = (record.hitLocation - center) / radius;
    record.setFaceNormal(r, outwardNormal);
    record.materialPointer = materialPointer;

    return true;
}

#endif //SPHERE_HPP