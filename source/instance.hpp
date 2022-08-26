#ifndef INSTANCE_HPP
#define INSTANCE_HPP

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
public:
    std::shared_ptr<hittable> object;
    glm::vec3 rotation;

private:
    rotate(){}

    rotate(const glm::vec3& rotation, std::shared_ptr<hittable> object):
        rotation{rotation},
        object{object}
        {}

    rotate(float x, float y, float z, std::shared_ptr<hittable> object):
        rotation{glm::vec3(x,y,z)},
        object{object}
        {}

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override {
        
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override {
        if(!object->boundingBox(tStart, tEnd, refbox))
            return false;

        // refbox.rotate(rotation);
        // return true;
    }
};

#endif //INSTANCE_HPP