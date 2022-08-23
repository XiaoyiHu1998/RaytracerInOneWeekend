#ifndef HITTABLE_LIST_HPP
#define HITTABLE_LIST_HPP

#include <memory>
#include <vector>
#include "hittable.hpp"

class hittableList : public hittable {
private:
    std::vector<std::shared_ptr<hittable>> objects; 
public:
    hittableList(){}
    hittableList(std::shared_ptr<hittable> object) { add(object); }
    void clear() { objects.clear(); }
    void add(std::shared_ptr<hittable> object) { objects.push_back(object); }

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override;
    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override;

    std::vector<std::shared_ptr<hittable>> objectList() const {
        return objects;
    }
};

bool hittableList::hit(const ray& r, float distMin, float distMax, hitRecord& record) const {
    hitRecord tempRecord;
    bool hit = false;
    float closestHitDist = distMax;

    for(const auto& object : objects){
        if(object->hit(r, distMin, closestHitDist, tempRecord)){
            hit = true;
            closestHitDist = tempRecord.distance;
            record = tempRecord;
        }
    }
    
    return hit;
}

bool hittableList::boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const {
    if(objects.empty())
        return false;

    axisAlignedBoundingBox tempbox = axisAlignedBoundingBox();
    bool firstBox = true;

    for(const std::shared_ptr<hittable>& objectPointer : objects){
        if(!objectPointer->boundingBox(tStart, tEnd, tempbox)) return false;
        refbox = firstBox ? tempbox : surroundingBox(tempbox, refbox);
        firstBox = false;
    }

    return true;
}

#endif //HITTABLE_LIST_HPP