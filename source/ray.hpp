#ifndef RAY_HPP
#define RAY_HPP

#include "vec3.hpp"

class ray{
private:
    point3 originInternal;
    vec3 directionInternal;

public:
    ray(){}
    
    ray(const point3& origin, const vec3& direction):
        originInternal{origin},
        directionInternal{direction}
        {}

    point3 origin() const {
        return originInternal;
    }    

    vec3 direction() const {
        return directionInternal;
    }

    point3 at(double t) const {
        return originInternal + t * directionInternal;
    }
};

#endif //RAY_HPP