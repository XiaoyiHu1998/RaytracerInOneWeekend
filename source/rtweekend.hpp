#ifndef RT_WEEKEND_HPP
#define RT_WEEKEND_HPP

#include <cmath>
#include <limits>
#include <memory>
#include <random>

//Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;
std::random_device sharedRandomDevice;
std::mt19937 sharedRng(sharedRandomDevice());

//Utility functions
inline double degrees_to_radians(double degrees){
    return degrees * pi / 180.0;
}

inline double randomDouble(std::mt19937& rng, double inclusiveMin = 0.0, double exclusiveMax = 1.0){
    std::uniform_real_distribution<double> distribution(inclusiveMin, exclusiveMax);
    return distribution(rng);
}

inline int randomInt(std::mt19937& rng, int inclusiveMin = 0, int exclusiveMax = 1){
    std::uniform_real_distribution<double> distribution(inclusiveMin, exclusiveMax);
    return static_cast<int>(distribution(rng));
}

inline double clamp(double x, double min, double max){
    if(x < min) return min;
    if(x > max) return max;
    return x;
}

//Common headers
#include "ray.hpp"
#include "vec3.hpp"

#endif //RT_WEEKEND_HPP