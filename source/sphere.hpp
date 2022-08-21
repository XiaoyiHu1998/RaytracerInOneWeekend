#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hittable.hpp"
#include "vec3.hpp"
#include <cmath>

class sphere : public hittable{
private:
    vec3 startCenter;
    vec3 endCenter;
    double startTime;
    double endTime;
    double radius;
    std::shared_ptr<material> materialPointer;

    vec3 center(double time) const {
        return startCenter + ((time - startTime) / (endTime - startTime)) * (endCenter - startCenter);  
    }

    void getSphereUV(const vec3 hitPoint, double& u, double& v) const {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    
        u = (atan2(-hitPoint.z(), hitPoint.x()) + pi) / (2 * pi);
        v = acos(-hitPoint.y()) / pi;
    }

public:
    // sphere(){}
    
    sphere(vec3 startCenter, vec3 endCenter, double radius, double startTime, double endTime, std::shared_ptr<material> materialPointer):
        startCenter{startCenter},
        endCenter{endCenter},
        startTime{startTime},
        endTime{endTime},
        radius{radius},
        materialPointer{materialPointer}
        {}
    
    sphere(vec3 startCenter, double radius, std::shared_ptr<material> materialPointer):
        startCenter{startCenter},
        endCenter{startCenter},
        startTime{0},
        endTime{1},
        radius{radius},
        materialPointer{materialPointer}
        {}

    virtual bool hit(const ray& r, double distMin, double distMax, hitRecord& record) const override;
    virtual bool boundingBox(double tStart, double tEnd, axisAlignedBoundingBox& refBox) const override;
};

bool sphere::hit(const ray& r, double distMin, double distMax, hitRecord& record) const {
    vec3 originMinCenter = r.origin() - center(r.hitTime());
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
    vec3 outwardNormal = (record.hitLocation - center(r.hitTime())) / radius;
    record.setFaceNormal(r, outwardNormal);
    record.materialPointer = materialPointer;
    getSphereUV(outwardNormal, record.u, record.v);

    return true;
}

bool sphere::boundingBox(double tStart, double tEnd, axisAlignedBoundingBox& refbox) const {
    axisAlignedBoundingBox tStartBox = axisAlignedBoundingBox(center(tStart) - vec3(radius, radius, radius), center(tStart) + vec3(radius, radius, radius));
    axisAlignedBoundingBox tEndBox   = axisAlignedBoundingBox(center(tEnd) - vec3(radius, radius, radius), center(tEnd) + vec3(radius, radius, radius));

    refbox = surroundingBox(tStartBox, tEndBox);
    return true;
}

#endif //SPHERE_HPP