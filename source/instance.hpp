#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <limits>
#include "hittable.hpp"

class translate : public hittable{
private:
    std::shared_ptr<hittable> object;
    glm::vec3 translation;

public:
    translate(){}

    translate(const glm::vec3& translation, std::shared_ptr<hittable> object):
        translation{translation},
        object{object}
        {}

    translate(float x, float y, float z, std::shared_ptr<hittable> object):
        translation{glm::vec3(x,y,z)},
        object{object}
        {}
    
    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override {
        ray rayTranslated(r.origin() - translation, r.direction(), r.hitTime());
        if(!object->hit(rayTranslated, distMin, distMax, record))
            return false;

        record.hitLocation += translation;
        record.setFaceNormal(rayTranslated, record.normal);

        return true;
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override {
        if(!object->boundingBox(tStart, tEnd, refbox))
            return false;

        refbox.translate(translation);
        return true;
    }
};

class rotate : public hittable {
private:
    std::shared_ptr<hittable> object;
    glm::vec3 rotation;
    bool hasBoundingBox;
    axisAlignedBoundingBox rotatedBoundingBox;

    inline glm::vec3 rotateX(const glm::vec3& direction, bool reverseRotate) const {
        float theta = degrees_to_radians(rotation.x);
        theta = reverseRotate ? -theta : theta;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        float yNew = cosTheta * direction.y - sinTheta * direction.z;
        float zNew = sinTheta * direction.y + cosTheta * direction.z;

        return glm::vec3(direction.x, yNew, zNew);
    }

    inline glm::vec3 rotateY(const glm::vec3& direction, bool reverseRotate) const {
        float theta = degrees_to_radians(rotation.y);
        theta = reverseRotate ? -theta : theta;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        float xNew =  cosTheta * direction.x + sinTheta * direction.z;
        float zNew = -sinTheta * direction.x + cosTheta * direction.z;

        return glm::vec3(xNew, direction.y, zNew);
    }

    inline glm::vec3 rotateZ(const glm::vec3& direction, bool reverseRotate) const {
        float theta = degrees_to_radians(rotation.z);
        theta = reverseRotate ? -theta : theta;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        float xNew = cosTheta * direction.x - sinTheta * direction.y;
        float yNew = sinTheta * direction.x + cosTheta * direction.y;

        return glm::vec3(xNew, yNew, direction.z);
    }

    glm::vec3 applyRotation(const point3& point) const {
        return rotateX(rotateY(rotateZ(point, false), false), false);
    }

    glm::vec3 applyReverseRotation(const point3& point) const {
        return rotateX(rotateY(rotateZ(point, true), true), true);
    }

public:
    rotate(){}

    rotate(const glm::vec3& rotation, std::shared_ptr<hittable> object):
        rotation{rotation},
        object{object}
        {
            hasBoundingBox = object->boundingBox(0, 1, rotatedBoundingBox);

            float inf = std::numeric_limits<float>::infinity();
            point3 min(inf, inf, inf);
            point3 max(-inf, -inf, -inf);

            for(int i = 0; i < 2; i++){
                for(int j = 0; j < 2; j++){
                    for(int k = 0; k < 2; k++){
                        float x = i * rotatedBoundingBox.cornerFarthest().x + (1 - i) * rotatedBoundingBox.cornerClosest().x;
                        float y = j * rotatedBoundingBox.cornerFarthest().y + (1 - j) * rotatedBoundingBox.cornerClosest().y;
                        float z = k * rotatedBoundingBox.cornerFarthest().z + (1 - k) * rotatedBoundingBox.cornerClosest().z;
                        
                        glm::vec3 oldPosition(x,y,z);
                        glm::vec3 newPosition = applyRotation(oldPosition);

                        for(int c = 0; c < 3; c++){
                            min[c] = fmin(min[c], newPosition[c]);
                            max[c] = fmax(max[c], newPosition[c]);
                        }
                    }
                }
            }

            rotatedBoundingBox = axisAlignedBoundingBox(min, max);
        }

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override {
        point3 originRotated = applyReverseRotation(r.origin());
        glm::vec3 directionRotated = applyReverseRotation(r.direction());
        ray rayRotated = ray(originRotated, directionRotated, r.hitTime());

        if(!object->hit(rayRotated, distMin, distMax, record));
            return false;

        record.hitLocation = applyRotation(record.hitLocation);
        record.setFaceNormal(rayRotated, applyRotation(record.normal));

        return true;
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override {
        refbox = rotatedBoundingBox;
        return hasBoundingBox;
    }
};

#endif //INSTANCE_HPP