#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>
#include "rtweekend.hpp"

class vec3{
private:
    double values[3];
public:
    vec3():
        values{0,0,0}
        {}
    
    vec3(double x, double y, double z):
        values{x,y,z}
        {}
    
    double x() const { return values[0]; }
    double y() const { return values[1]; }
    double z() const { return values[2]; }

    inline static vec3 random(double min = 0.0, double max = 1.0){
        return vec3(randomDouble(sharedRng, min, max), randomDouble(sharedRng, min, max), randomDouble(sharedRng, min, max));
    }

    inline bool nearZero() const{
        const double s = 1e-8;
        return (fabs(values[0]) < s) && (fabs(values[1]) < s) && (fabs(values[2]) < s);
    }

    vec3 operator-() const {return vec3(-values[0], -values[1], -values[2]);}
    inline double operator[](int i) const {return values[i];}
    inline double& operator[](int i) {return values[i];}

    vec3& operator+=(const vec3& other){
        values[0] += other[0];
        values[1] += other[1];
        values[2] += other[2];

        return *this;
    }

    vec3& operator-=(const vec3& other){
        values[0] -= other[0];
        values[1] -= other[1];
        values[2] -= other[2];

        return *this;
    }

    vec3& operator*=(const double n){
        values[0] *= n;
        values[1] *= n;
        values[2] *= n;
        
        return *this;
    }

    vec3& operator/=(const double n){
        return *this *= 1/n;
    }

    double length(){
        return std::sqrt(lengthSquared());
    }

    inline double lengthSquared(){
        return values[0] * values[0] + values[1] * values[1] + values[2] * values[2];
    }

};

using point3 = vec3;
using color = vec3;

//vec3 utility functions

inline std::ostream& operator<<(std::ostream &out, const vec3& v){
    return out << "vec3[" << v[0] << ',' << v[1] << ',' << v[2] << ']';
}

inline vec3 operator+(const vec3& lhs, const vec3& rhs){
    return vec3(lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]);
}

inline vec3 operator-(const vec3& lhs, const vec3& rhs){
    return vec3(lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v){
    return vec3(u[0] * v[0], u[1] * v[1], u[2] * v[2]);
}

inline vec3 operator*(double n, const vec3& v){
    return vec3(v[0] * n, v[1] * n, v[2] * n);
}

inline vec3 operator*(const vec3& v, double n){
    return n * v;
}

inline vec3 operator/(const vec3& v, const double n){
    return (1.0/n) * v;
}

inline double dot(const vec3& u, const vec3& v){
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

inline vec3 cross(const vec3& u, const vec3& v){
    return vec3(u[1] * v[2] - u[2] * v[1],
                u[2] * v[0] - u[0] * v[2],
                u[0] * v[1] - u[1] * v[0]);
}

inline vec3 normalize(vec3 v){
    return v / v.length();
}

vec3 randomInUnitSphere(){
    while(true){
        vec3 insideSphereVector = vec3::random(-1.0, 1.0);
        if(insideSphereVector.lengthSquared() < 1);
            return insideSphereVector;
    }
}

vec3 randomUnitVector(){
    return normalize(randomInUnitSphere());
}

vec3 randomInHemisphere(const vec3& faceNormal){
    vec3 inUnitSphere = randomInUnitSphere();
    double dotproduct = dot(inUnitSphere, faceNormal);
    return (dotproduct / std::fabs(dotproduct)) * inUnitSphere;
}

vec3 randomInUnitDisk(){
    while(true){
        point3 point = vec3(randomDouble(sharedRng, -1.0, 1.0), randomDouble(sharedRng, -1.0, 1.0), 0);
        if(point.lengthSquared() < 1)
            return point;
    }
}

vec3 reflect(const vec3& incomingVector, const vec3& normalVector){
    vec3 incomingUnitVector = normalize(incomingVector);
    return incomingUnitVector - 2 * dot(incomingUnitVector, normalVector) * normalVector;
}

vec3 refract(const vec3& incomingVector, const vec3& faceNormal, double etaiOverEtat){
    vec3 incomingUnitVector = normalize(incomingVector);

    double cosTheta = std::fmin(dot(-incomingUnitVector, faceNormal), 1.0);
    vec3 rayOutPerpendicular = etaiOverEtat * (incomingUnitVector + cosTheta * faceNormal);
    vec3 rayOutParallel = -std::sqrt(fabs(1.0 - rayOutPerpendicular.lengthSquared())) * faceNormal;
    return rayOutPerpendicular + rayOutParallel;
}

#endif //VEC3_HPP