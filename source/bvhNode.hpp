#ifndef BVH_NODE
#define BVH_NODE

#include <iostream>
#include <algorithm>
#include "rtweekend.hpp"
#include "hittable.hpp"
#include "hittableList.hpp"

enum class bvhAxis{
    x, y, z
};

class bvhNode : public hittable {
public:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    axisAlignedBoundingBox nodeBoundingBox;

    bvhNode(){}

    bvhNode(const hittableList& list, float tStart = 0.0, float tEnd = 1.0):
        bvhNode{list.objectList(), 0, static_cast<uint64_t>(list.objectList().size()), tStart, tEnd, bvhAxis::x}
        {}

    bvhNode(const std::vector<std::shared_ptr<hittable>> objectListConst, uint64_t listStart, uint64_t listEnd, float tStart, float tEnd, bvhAxis axis);

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override;
    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override;
};

inline bool boxCompare(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b, int axis){
    axisAlignedBoundingBox boxA;
    axisAlignedBoundingBox boxB;

    if(!a->boundingBox(0, 0, boxA) || !b->boundingBox(0, 0, boxB))
        std::cerr << "No bounding box in bvhNode constructor.\n";

    return boxA.cornerClosest()[axis] < boxB.cornerClosest()[axis];
}

bool boxCompareX(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b){
    return boxCompare(a, b, 0);
}

bool boxCompareY(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b){
    return boxCompare(a, b, 1);
}

bool boxCompareZ(const std::shared_ptr<hittable> a, const std::shared_ptr<hittable> b){
    return boxCompare(a, b, 2);
}

bvhNode::bvhNode(const std::vector<std::shared_ptr<hittable>> objectListConst, uint64_t listStart, uint64_t listEnd, float tStart, float tEnd, bvhAxis axis){
    std::vector<std::shared_ptr<hittable>> objectList = objectListConst;

    bvhAxis nextAxis;
    auto comparator = boxCompareX;

    switch(axis){
        case bvhAxis::x:
            nextAxis = bvhAxis::y;
            break;
        case bvhAxis::y:
            nextAxis = bvhAxis::z;
            boxCompareY;
            break;
        case bvhAxis::z:
            nextAxis = bvhAxis::x;
            boxCompareZ;
            break;
    }

    uint64_t listLength = listEnd - listStart;

    if(listLength == 1){
        left = objectList[listStart];
        right = left;
    }
    else if(listLength == 2){
        if(comparator(objectList[listStart], objectList[listEnd - 1])){
            left = objectList[listStart];
            right = objectList[listEnd - 1];
        }
        else{
            left = objectList[listEnd - 1];
            right = objectList[listStart];
        }
    }
    else{
        std::sort(objectList.begin() + listStart, objectList.begin() + listEnd, comparator);

        int middle = listStart + listLength / 2;
        left = std::make_shared<bvhNode>(objectListConst, listStart, middle, tStart, tEnd, nextAxis);
        right = std::make_shared<bvhNode>(objectListConst, middle, listEnd, tStart, tEnd, nextAxis);
    }

    axisAlignedBoundingBox leftBox, rightBox;

    if(!left->boundingBox(tStart, tEnd, leftBox) || !right->boundingBox(tStart, tEnd, rightBox)){
        std::cerr << "No bounding box in bvhNode constructor.\n";
    }

    nodeBoundingBox = surroundingBox(leftBox, rightBox);
}

bool bvhNode::hit(const ray& r, float distMin, float distMax, hitRecord& record) const {
    if(!nodeBoundingBox.hit(r, distMin, distMax))
        return false;
    
    bool hitLeft = left->hit(r, distMin, distMax, record);
    bool hitRight = right->hit(r, distMin, hitLeft ? record.distance : distMax, record);

    return hitLeft || hitRight;
}

bool bvhNode::boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const {
    refbox = nodeBoundingBox;
    return true;
}

#endif //BVH_NODE