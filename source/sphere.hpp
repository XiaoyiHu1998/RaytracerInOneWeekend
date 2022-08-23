#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "hittable.hpp"
#include "vec3.hpp"
#include <cmath>

class sphere : public hittable{
private:
    glm::vec3 startCenter;
    glm::vec3 endCenter;
    float startTime;
    float endTime;
    float radius;
    std::shared_ptr<material> materialPointer;

    glm::vec3 center(float time) const {
        return startCenter + ((time - startTime) / (endTime - startTime)) * (endCenter - startCenter);  
    }

    void getSphereUV(const glm::vec3 hitPoint, float& u, float& v) const {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
    
        u = (atan2(-hitPoint.z, hitPoint.x) + pi) / (2 * pi);
        v = acos(-hitPoint.y) / pi;
    }

public:
    // sphere(){}
    
    sphere(glm::vec3 startCenter, glm::vec3 endCenter, float radius, float startTime, float endTime, std::shared_ptr<material> materialPointer):
        startCenter{startCenter},
        endCenter{endCenter},
        startTime{startTime},
        endTime{endTime},
        radius{radius},
        materialPointer{materialPointer}
        {}
    
    sphere(glm::vec3 startCenter, float radius, std::shared_ptr<material> materialPointer):
        startCenter{startCenter},
        endCenter{startCenter},
        startTime{0},
        endTime{1},
        radius{radius},
        materialPointer{materialPointer}
        {}

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override;
    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refBox) const override;
};

bool sphere::hit(const ray& r, float distMin, float distMax, hitRecord& record) const {
    glm::vec3 originMinCenter = r.origin() - center(r.hitTime());
    glm::vec3 rayDirection = r.direction();
    float a = lengthSquared(rayDirection);
    float bHalf = dot(rayDirection, originMinCenter);
    float c = lengthSquared(originMinCenter) - radius * radius;
    float discriminant = bHalf*bHalf - a*c;
    
    if(discriminant < 0.0)
        return false;

    float sqrtDiscriminant = std::sqrt(discriminant);
    float hitDistance = (-bHalf - sqrtDiscriminant) / a;
    //TODO: check if performance increases when you remove the extra branch
    if(hitDistance < distMin || hitDistance > distMax){
        hitDistance = (-bHalf + sqrtDiscriminant) / a;
        if(hitDistance < distMin || hitDistance > distMax)
            return false;
    }

    record.distance = hitDistance;
    record.hitLocation = r.at(hitDistance);
    glm::vec3 outwardNormal = (record.hitLocation - center(r.hitTime())) / radius;
    record.setFaceNormal(r, outwardNormal);
    record.materialPointer = materialPointer;
    getSphereUV(outwardNormal, record.u, record.v);

    return true;
}

bool sphere::boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const {
    axisAlignedBoundingBox tStartBox = axisAlignedBoundingBox(center(tStart) - glm::vec3(radius, radius, radius), center(tStart) + glm::vec3(radius, radius, radius));
    axisAlignedBoundingBox tEndBox   = axisAlignedBoundingBox(center(tEnd) - glm::vec3(radius, radius, radius), center(tEnd) + glm::vec3(radius, radius, radius));

    refbox = surroundingBox(tStartBox, tEndBox);
    return true;
}

#endif //SPHERE_HPP