#ifndef AXIS_ALIGNED_BOUNDING_BOX
#define AXIS_ALIGNED_BOUNDING_BOX

#include "vec3.hpp"
#include <cmath>

class axisAlignedBoundingBox{
private:
    vec3 cornerClosestZ;
    vec3 cornerFarthestZ;

public:
    axisAlignedBoundingBox(){}
    axisAlignedBoundingBox(const vec3& corner1, const vec3& corner2){
        cornerClosestZ  = corner1.z() <= corner2.z() ? corner1 : corner2;
        cornerFarthestZ = corner1.z() > corner2.z() ? corner1 : corner2;
    }

    vec3 cornerClosest()  const { return cornerClosestZ; }
    vec3 cornerFarthest() const { return cornerFarthestZ; }

    bool hit(const ray& r, double tMin, double tMax) const {
        for(int dimension = 0; dimension < 3; dimension++){
            double invD = 1.0f / r.direction()[dimension];
            double t0 = (cornerClosestZ[dimension] - r.origin()[dimension]) * invD;
            double t1 = (cornerFarthestZ[dimension] - r.origin()[dimension]) * invD;

            if (invD < 0.0f)
                std::swap(t0, t1);

            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;

            if(tMax <= tMin)
                return false;
        }

        return true;
    }


};

axisAlignedBoundingBox surroundingBox(const axisAlignedBoundingBox& box1, const axisAlignedBoundingBox& box2) {
    vec3 cornerClosest = vec3(fmin(box1.cornerClosest().x(), box2.cornerClosest().x()),
                              fmin(box1.cornerClosest().y(), box2.cornerClosest().y()),
                              fmin(box1.cornerClosest().z(), box2.cornerClosest().z()));

    vec3 cornerFarthest = vec3(fmax(box1.cornerFarthest().x(), box2.cornerFarthest().x()),
                               fmax(box1.cornerFarthest().y(), box2.cornerFarthest().y()),
                               fmax(box1.cornerFarthest().z(), box2.cornerFarthest().z()));

    return axisAlignedBoundingBox(cornerClosest, cornerFarthest);
}

#endif //AXIS_ALIGNED_BOUNDING_BOX