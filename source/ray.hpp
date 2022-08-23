#ifndef RAY_HPP
#define RAY_HPP

#include "vec3.hpp"

class ray{
private:
    point3 originInternal;
    glm::vec3 directionInternal;
    double timePoint;

public:
    ray(){}
    
    ray(const point3& origin, const glm::vec3& direction, double time = 0.0):
        originInternal{origin},
        directionInternal{direction},
        timePoint{time}
        {}

    point3 origin() const {
        return originInternal;
    }    

    glm::vec3 direction() const {
        return directionInternal;
    }

    point3 at(float t) const {
        return originInternal + t * directionInternal;
    }

    double hitTime() const {
        return timePoint;
    }
};

#endif //RAY_HPP