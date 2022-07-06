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

    virtual bool hit(const ray& r, double distMin, double distMax, hitRecord& record) const override;
};

bool hittableList::hit(const ray& r, double distMin, double distMax, hitRecord& record) const {
    hitRecord tempRecord;
    bool hit = false;
    double closestHitDist = distMax;

    for(const auto& object : objects){
        if(object->hit(r, distMin, closestHitDist, tempRecord)){
            hit = true;
            closestHitDist = tempRecord.distance;
            record = tempRecord;
        }
    }
    
    return hit;
}

#endif //HITTABLE_LIST_HPP