#ifndef AXIS_ALIGNED_BOUNDING_BOX
#define AXIS_ALIGNED_BOUNDING_BOX

#include "vec3.hpp"
#include <cmath>

class axisAlignedBoundingBox{
private:
    point3 cornerClosestZ;
    point3 cornerFarthestZ;

public:
    axisAlignedBoundingBox(){}
    axisAlignedBoundingBox(const point3& corner1, const point3& corner2){
        cornerClosestZ  = corner1.z <= corner2.z ? corner1 : corner2;
        cornerFarthestZ = corner1.z > corner2.z ? corner1 : corner2;
    }

    point3 cornerClosest()  const { return cornerClosestZ; }
    point3 cornerFarthest() const { return cornerFarthestZ; }
    void translate(const glm::vec3& translation) { cornerClosestZ += translation; cornerFarthestZ += translation; }

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

    friend axisAlignedBoundingBox surroundingBox(const axisAlignedBoundingBox& box1, const axisAlignedBoundingBox& box2);
};

axisAlignedBoundingBox surroundingBox(const axisAlignedBoundingBox& box1, const axisAlignedBoundingBox& box2) {
    glm::vec3 cornerClosest = glm::vec3(
                              fmin(box1.cornerClosestZ.x, box2.cornerClosestZ.x),
                              fmin(box1.cornerClosestZ.y, box2.cornerClosestZ.y),
                              fmin(box1.cornerClosestZ.z, box2.cornerClosestZ.z));

    glm::vec3 cornerFarthest = glm::vec3(
                               fmax(box1.cornerFarthestZ.x, box2.cornerFarthestZ.x),
                               fmax(box1.cornerFarthestZ.y, box2.cornerFarthestZ.y),
                               fmax(box1.cornerFarthestZ.z, box2.cornerFarthestZ.z));

    return axisAlignedBoundingBox(cornerClosest, cornerFarthest);
}

#endif //AXIS_ALIGNED_BOUNDING_BOX