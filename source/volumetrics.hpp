#ifndef VOLUMETRICS_HPP
#define VOLUMETRICS_HPP

#include "rtweekend.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "texture.hpp"

class constantMedium : public hittable{
private:
    std::shared_ptr<hittable> boundary;
    std::shared_ptr<material> phaseFunction;
    float negativeInverseDensity;

public:
    constantMedium(std::shared_ptr<hittable> boundary, float density, std::shared_ptr<texture> texture):
        boundary{boundary},
        negativeInverseDensity{-1.0f/density},
        phaseFunction{std::shared_ptr<isotropic>(texture)}
        {}

    constantMedium(std::shared_ptr<hittable> boundary, float density, color volumeColor):
        boundary{boundary},
        negativeInverseDensity{1.0f/density},
        phaseFunction{std::make_shared<isotropic>(volumeColor)}
        {}
    
    virtual bool hit(const ray& ray, float distMin, float distMax, hitRecord& record) const override {
        #ifdef DEBUG
        const bool debugPrinting = false;
        const bool debugging = debugPrinting && randomFloat() < 0.000001;
        #endif

        hitRecord record1, record2;
        float inf = std::numeric_limits<float>::infinity();

        bool record1hit = boundary->hit(r, -inf, inf, record1);
        bool record2hit = boundary->hit(r, record.distance + 0.0001, inf, record2);

        if(!record1hit || !record2hit)
            return false;

        #ifdef DEBUG
        if(debugging)
            std::cerr << "\ndistMin: " << record1.distance << ", distMax: " << record2.distance << "\n";
        #endif
        
        record1.distance = std::max(record1.distance, distMin);
        record2.distance = std::min(record2.distance, distMax);

        if(record1.distance >= record2.distance)
            return false;

        record1.distance = std::min(record1.distance, 0);

        const float raySpeed = ray.direction().length();
        const float distanceInBoundary = (record2.distance - record1.distance) * raySpeed;
        const float hitDistance = negativeInverseDensity * log(randomFloat());

        if(hitDistance > distanceInBoundary)
            return false;

        record.distance = record1.distance / raySpeed;
        record.hitLocation = ray.at(record.distance);
        
        #ifdef DEBUG
        if(debugging){
            std::cerr << "hit_distance = " <<  hit_distance << '\n'
                      << "record.distance = " <<  record.distance << '\n'
                      << "record.hitLocation = " <<  record.hitLocation << '\n';
        }
        #endif

        record.normal = vec3(1,0,0); //some arbitrary normal
        record.frontFace = true; // another arbitrary value
        record.materialPointer = phaseFunction;

        return true;
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override {
        return boundary->boundingBox(tStart, tEnd, refbox);
    }
};

#endif //VOLUMETRICS_HPP