#ifndef BVH_NODE
#define BVH_NODE

#include <iostream>
#include <algorithm>
#include "rtweekend.hpp"
#include "hittable.hpp"
#include "hittableList.hpp"

class bvhNode : public hittable {
public:
    std::shared_ptr<hittable> left;
    std::shared_ptr<hittable> right;
    axisAlignedBoundingBox nodeBoundingBox;

    bvhNode(){}

    bvhNode(const hittableList& list, double tStart, double tEnd):
        bvhNode{list.objectList(), 0, list.objectList().size(), tStart, tEnd}
        {}

    bvhNode(const std::vector<std::shared_ptr<hittable>> objectListConst, int listStart, int listEnd, double tStart, double tEnd);

    virtual bool hit(const ray& r, double distMin, double distMax, hitRecord& record) const override;
    virtual bool boundingBox(double tStart, double tEnd, axisAlignedBoundingBox& refbox) const override;
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

bvhNode::bvhNode(const std::vector<std::shared_ptr<hittable>> objectListConst, int listStart, int listEnd, double tStart, double tEnd){
    std::vector<std::shared_ptr<hittable>> objectList = objectListConst;

    int axis = randomInt(sharedRandomDevice, 0, 3);
    auto comparator = (axis == 0) ? boxCompareX : (axis == 1) ? boxCompareY : boxCompareZ;

    size_t listLength = listEnd - listStart;

    if(listLength == 1){
        left = objectList[listStart];
        right = left;
    }
    else if(listLength == 2){
        if(comparator(objectList[listStart], objectList[listEnd])){
            left = objectList[listStart];
            right = objectList[listEnd];
        }
        else{
            left = objectList[listEnd];
            right = objectList[listStart];
        }
    }
    else{
        std::sort(objectList.begin() + listStart, objectList.begin() + listEnd, comparator);

        int middle = listStart + listLength / 2;
        left = std::make_shared<bvhNode>(objectListConst, listStart, middle, tStart, tEnd);
        right = std::make_shared<bvhNode>(objectListConst, middle, listEnd, tStart, tEnd);
    }

    axisAlignedBoundingBox leftBox, rightBox;

    if(!left->boundingBox(tStart, tEnd, leftBox) || !right->boundingBox(tStart, tEnd, rightBox)){
        std::cerr << "No bounding box in bvhNode constructor.\n";
    }

    nodeBoundingBox = surroundingBox(leftBox, rightBox);
}

bool bvhNode::hit(const ray& r, double distMin, double distMax, hitRecord& record) const {
    if(!nodeBoundingBox.hit(r, distMin, distMax))
        return false;
    
    bool hitLeft = left->hit(r, distMin, distMax, record);
    bool hitRight = right->hit(r, distMin, distMax, record);

    return hitLeft || hitRight;
}

bool bvhNode::boundingBox(double tStart, double tEnd, axisAlignedBoundingBox& refbox) const {
    refbox = nodeBoundingBox;
    return true;
}

#endif //BVH_NODE