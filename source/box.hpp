#ifndef BOX_HPP
#define BOX_HPP

#include "rtweekend.hpp"
#include "hittable.hpp"
#include "hittableList.hpp"
#include "rectangle.hpp"

class box : public hittable{
private:
    point3 minCorner;
    point3 maxCorner;
    hittableList sides;
    std::shared_ptr<material> mat;

public:
    box(){}

    box(const point3& minCorner, const point3& maxCorner, std::shared_ptr<material> mat, float tStart = 0.0, float tEnd = 1.0, const glm::vec3& displacement = glm::vec3(0,0,0)):
    minCorner{minCorner},
    maxCorner{maxCorner}
    {
        sides.add(std::make_shared<rectangleXY>(minCorner.x, maxCorner.x, minCorner.y, maxCorner.y, maxCorner.z, mat, tStart, tEnd, displacement));
        sides.add(std::make_shared<rectangleXY>(minCorner.x, maxCorner.x, minCorner.y, maxCorner.y, minCorner.z, mat, tStart, tEnd, displacement));

        sides.add(std::make_shared<rectangleXZ>(minCorner.x, maxCorner.x, minCorner.z, maxCorner.z, maxCorner.y, mat, tStart, tEnd, displacement));
        sides.add(std::make_shared<rectangleXZ>(minCorner.x, maxCorner.x, minCorner.z, maxCorner.z, minCorner.y, mat, tStart, tEnd, displacement));

        sides.add(std::make_shared<rectangleYZ>(minCorner.y, maxCorner.y, minCorner.z, maxCorner.z, maxCorner.x, mat, tStart, tEnd, displacement));
        sides.add(std::make_shared<rectangleYZ>(minCorner.y, maxCorner.y, minCorner.z, maxCorner.z, minCorner.x, mat, tStart, tEnd, displacement));
    }

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override {
        return sides.hit(r, distMin, distMax, record);
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override {
        refbox = axisAlignedBoundingBox(minCorner, maxCorner);
        return true;
    }

};

#endif //BOX_HPP