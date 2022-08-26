#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "hittable.hpp"

class rectangleXY : public hittable{
private:
    float leftX, rightX;
    float bottomY, topY;
    float constZ;
    float tStart,tEnd;
    glm::vec3 displacement;
    std::shared_ptr<material> mat;

public:
    rectangleXY(float leftX, float rightX, float bottomY, float topY, float constZ, std::shared_ptr<material> mat, float tStart = 0.0, float tEnd = 1.0, const glm::vec3& displacement = glm::vec3(0,0,0)):
    leftX{leftX}, rightX{rightX},
    bottomY{bottomY}, topY{topY},
    constZ{constZ},
    mat{mat},
    tStart{tStart}, tEnd{tEnd},
    displacement{displacement}
    {}

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override{
        glm::vec3 delta = std::min(1.0f, ((r.hitTime() - tStart) / (tEnd - tStart))) * displacement;
        point3 origin = r.origin();
        glm::vec3 rayDirection = r.direction();
        float distance = (constZ + delta.z - origin.z) / rayDirection.z;
        if(distance < distMin || distance > distMax)
            return false;

        float rayX = origin.x + rayDirection.x * distance;
        float rayY = origin.y + rayDirection.y * distance;
        if(rayX < leftX + delta.x || rayX > rightX + delta.x || rayY < bottomY + delta.y || rayY > topY + delta.y)
            return false;

        record.distance = distance;
        record.hitLocation = r.at(distance);
        record.materialPointer = mat;
        record.setFaceNormal(r, glm::vec3(0,0,1));
        record.u = (rayX - leftX) / (rightX - leftX);
        record.v = (rayY - bottomY) / (topY - bottomY);

        return true;
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override{
        float epsilon = 0.0001;
        axisAlignedBoundingBox box1(glm::vec3(leftX, bottomY, constZ - epsilon), glm::vec3(rightX, topY, constZ + epsilon));
        axisAlignedBoundingBox box2(glm::vec3(leftX, bottomY, constZ - epsilon) + displacement, glm::vec3(rightX, topY, constZ + epsilon) + displacement);
        refbox = surroundingBox(box1, box2);
        
        return true;
    }

};

class rectangleXZ : public hittable{
private:
    float leftX, rightX;
    float frontZ, backZ;
    float constY;
    float tStart,tEnd;
    glm::vec3 displacement;
    std::shared_ptr<material> mat;

public:
    rectangleXZ(float leftX, float rightX, float frontZ, float backZ, float constY, std::shared_ptr<material> mat, float tStart = 0.0, float tEnd = 1.0, const glm::vec3& displacement = glm::vec3(0,0,0)):
    leftX{leftX}, rightX{rightX},
    frontZ{frontZ}, backZ{backZ},
    constY{constY},
    mat{mat},
    tStart{tStart}, tEnd{tEnd},
    displacement{displacement}
    {}

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override{
        glm::vec3 delta = std::min(1.0f, ((r.hitTime() - tStart) / (tEnd - tStart))) * displacement;
        point3 origin = r.origin();
        glm::vec3 rayDirection = r.direction();
        float distance = (constY + delta.y - origin.y) / rayDirection.y;
        if(distance < distMin || distance > distMax)
            return false;

        float rayX = origin.x + rayDirection.x * distance;
        float rayZ = origin.z + rayDirection.z * distance;
        if(rayX < leftX + delta.x || rayX > rightX + delta.x || rayZ < frontZ + delta.z || rayZ > backZ + delta.z)
            return false;

        record.distance = distance;
        record.hitLocation = r.at(distance);
        record.materialPointer = mat;
        record.setFaceNormal(r, glm::vec3(0,1,0));
        record.u = (rayX - leftX) / (rightX - leftX);
        record.v = (rayZ - frontZ) / (backZ - frontZ);

        return true;
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override{
        float epsilon = 0.0001;
        axisAlignedBoundingBox box1(glm::vec3(leftX, constY - epsilon, frontZ), glm::vec3(rightX, constY + epsilon, backZ));
        axisAlignedBoundingBox box2(glm::vec3(leftX, constY - epsilon, frontZ) + displacement, glm::vec3(rightX, constY + epsilon, backZ) + displacement);
        refbox = surroundingBox(box1, box2);
        
        return true;
    }
    
};

class rectangleYZ : public hittable{
private:
    float bottomY, topY;
    float frontZ, backZ;
    float constX;
    float tStart,tEnd;
    glm::vec3 displacement;
    std::shared_ptr<material> mat;

public:
    rectangleYZ(float bottomY, float topY, float frontZ, float backZ, float constX, std::shared_ptr<material> mat, float tStart = 0.0, float tEnd = 1.0, const glm::vec3& displacement = glm::vec3(0,0,0)):
    bottomY{bottomY}, topY{topY},
    frontZ{frontZ}, backZ{backZ},
    constX{constX},
    mat{mat},
    tStart{tStart}, tEnd{tEnd},
    displacement{displacement}
    {}

    virtual bool hit(const ray& r, float distMin, float distMax, hitRecord& record) const override{
        glm::vec3 delta = std::min(1.0f, ((r.hitTime() - tStart) / (tEnd - tStart))) * displacement;
        point3 origin = r.origin();
        glm::vec3 rayDirection = r.direction();
        float distance = (constX + delta.x - origin.x) / rayDirection.x;
        if(distance < distMin || distance > distMax)
            return false;

        float rayY = origin.y + rayDirection.y * distance;
        float rayZ = origin.z + rayDirection.z * distance;
        if(rayY < bottomY + delta.y || rayY > topY + delta.y || rayZ < frontZ + delta.z || rayZ > backZ + delta.z)
            return false;

        record.distance = distance;
        record.hitLocation = r.at(distance);
        record.materialPointer = mat;
        record.setFaceNormal(r, glm::vec3(1,0,0));
        record.u = (rayY - bottomY) / (topY - bottomY);
        record.v = (rayZ - frontZ) / (backZ - frontZ);

        return true;
    }

    virtual bool boundingBox(float tStart, float tEnd, axisAlignedBoundingBox& refbox) const override{
        float epsilon = 0.0001;
        axisAlignedBoundingBox box1(glm::vec3(constX - epsilon, bottomY, frontZ), glm::vec3(constX + epsilon, topY, backZ));
        axisAlignedBoundingBox box2(glm::vec3(constX - epsilon, bottomY, frontZ) + displacement, glm::vec3(constX + epsilon, topY, backZ) + displacement);
        refbox = surroundingBox(box1, box2);
        
        return true;
    }

};

class rectangle : public hittable{
private:
    point3 origin;
    glm::vec3 u;
    glm::vec3 v;

public:
    rectangle(){}
};


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

#endif //RECTANGLE_HPP