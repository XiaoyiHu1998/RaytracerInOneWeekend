#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <limits>
#include "hittable.hpp"
#include "glm/gtx/transform.hpp"

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

class rotate : public hittable{
private:
    std::shared_ptr<hittable> object;
    glm::vec3 rotation;
    float sinTheta[3];
    float cosTheta[3];
    bool hasBoundingBox;
    axisAlignedBoundingBox rotatedBoundingBox;

    inline glm::vec3 rotationX(const glm::vec3& position, float sign = 1.0) const {
        float signedSinTheta = sign * sinTheta[0];
        glm::vec3 positionRotated;

        positionRotated.x = position.x;
        positionRotated.y = cosTheta[0] * position.y - signedSinTheta * position.z;
        positionRotated.z = signedSinTheta * position.y + cosTheta[0] * position.z;
        
        return positionRotated;
    }

    inline glm::vec3 rotationY(const glm::vec3& position, float sign = 1.0) const {
        float signedSinTheta = sign * sinTheta[1];
        glm::vec3 positionRotated;

        positionRotated.x =  cosTheta[1] * position.x + signedSinTheta * position.z;
        positionRotated.y =  position.y;
        positionRotated.z = -signedSinTheta * position.x + cosTheta[1] * position.z;
        
        return positionRotated;
    }

    inline glm::vec3 rotationZ(const glm::vec3& position, float sign = 1.0) const {
        float signedSinTheta = sign * sinTheta[2];
        glm::vec3 positionRotated;

        positionRotated.x = cosTheta[2] * position.x - signedSinTheta * position.y;
        positionRotated.y = signedSinTheta * position.x + cosTheta[2] * position.y;
        positionRotated.z = position.z;
        
        return positionRotated;
    }

    inline glm::vec3 applyRotation(const glm::vec3& position) const {
        // return rotationX(rotationY(rotationZ(position)));
        return glm::vec3(glm::rotate(rotation.z, point3(0,0,1)) * glm::rotate(rotation.y, point3(0,1,0)) * glm::rotate(rotation.x, point3(1,0,0)) * glm::vec4(position,0));
    }

    inline glm::vec3 applyReverseRotation(const glm::vec3& position) const {
        // return rotationX(rotationY(rotationZ(position, -1.0f) -1.0f), -1.0f);
        return glm::vec3(glm::rotate(-rotation.z, point3(0,0,1)) * glm::rotate(-rotation.y, point3(0,1,0)) * glm::rotate(-rotation.x, point3(1,0,0)) * glm::vec4(position,0));
    }

public:
    rotate(glm::vec3 rotations, const std::shared_ptr<hittable>& objectPtr){
        object = objectPtr;
        rotation = rotations;

        for(int i = 0; i < 3; i++){
            rotation[i] = glm::radians(rotations[i]);
        }

        float thetaX = degrees_to_radians(rotation.x);
        float thetaY = degrees_to_radians(rotation.y);
        float thetaZ = degrees_to_radians(rotation.z);

        sinTheta[0] = sin(thetaX);
        sinTheta[1] = sin(thetaY);
        sinTheta[2] = sin(thetaZ);

        cosTheta[0] = cos(thetaX);
        cosTheta[1] = cos(thetaY);
        cosTheta[2] = cos(thetaZ);

        hasBoundingBox = object->boundingBox(0, 1, rotatedBoundingBox);

        float inf = std::numeric_limits<float>::infinity();
        point3 min( inf,  inf,  inf);
        point3 max(-inf, -inf, -inf);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*rotatedBoundingBox.cornerFarthest().x + (1-i)*rotatedBoundingBox.cornerClosest().x;
                    auto y = j*rotatedBoundingBox.cornerFarthest().y + (1-j)*rotatedBoundingBox.cornerClosest().y;
                    auto z = k*rotatedBoundingBox.cornerFarthest().z + (1-k)*rotatedBoundingBox.cornerClosest().z;

                    glm::vec3 position = applyRotation(glm::vec3(x,y,z));

                    for (int c = 0; c < 3; c++) {
                        min[c] = fmin(min[c], position[c]);
                        max[c] = fmax(max[c], position[c]);
                    }
                }
            }
        }

        rotatedBoundingBox = axisAlignedBoundingBox(min, max);
    }

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override {
        point3 originRotated = applyReverseRotation(r.origin());
        glm::vec3 directionRotated = applyReverseRotation(r.direction());

        ray rayRotated(originRotated, directionRotated, r.hitTime());
        if(!object->hit(rayRotated, distMin, distMax, record))
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