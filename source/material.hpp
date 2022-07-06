#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "rtweekend.hpp"
#include "hittable.hpp"

class material{
public:
    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const = 0;
    virtual color getAlbedo() const = 0;
};


namespace mat{
    
class lambertian : public material{
private:
    color albedo;
public:
    lambertian(const color& albedo):
        albedo{albedo}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override{
        vec3 scatterDirection;

        #ifdef IN_HEMISPHERE
            scatterDirection = randomInHemisphere(record.normal);
        #endif

        #ifdef UNIT_SPHERE
            scatterDirection = record.normal + randomInUnitSphere();
        #endif
        
        #ifdef UNIT_VECTOR
            scatterDirection = record.normal + randomUnitVector();
        #endif
        
        //catch potential cases where scatterDirection == vec3(0,0,0)
        if(scatterDirection.nearZero())
            scatterDirection = record.normal;

        rayScattered = ray(record.hitLocation, scatterDirection);
        attenuation = albedo;
        return true;
    }

    virtual color getAlbedo() const override{
        return albedo;
    }
};


class metal : public material{
private:
    color albedo;
    double roughness;
public:
    metal(const color& albedo, const double roughness):
        albedo{albedo},
        roughness{roughness < 1 ? roughness : 1.0}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override {
        vec3 reflectedDirection = reflect(rayIncoming.direction(), record.normal) + roughness * randomInUnitSphere();
        rayScattered = ray(record.hitLocation, reflectedDirection);
        attenuation = albedo;
        return dot(reflectedDirection, record.normal) > 0;
    }

    virtual color getAlbedo() const override{
        return albedo;
    }
};

class dielectric : public material{
private:
    color albedo;
    double refractionIndex;

    static double reflectance(double cosine, double refractionIndex){
        //schlicks approximation for reflectance
        double r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = std::pow(r0, 2);
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
public:
    dielectric(double refractionIndex):
        refractionIndex{refractionIndex},
        albedo{color(1,1,1)}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        double refractionRatio = record.frontFace ? (1.0 / refractionIndex) : refractionIndex;

        double cosTheta = std::fmin(dot(-normalize(rayIncoming.direction()), record.normal), 1.0);
        double sinTheta = std::sqrt(1.0 - (cosTheta * cosTheta));

        bool cantRefract = refractionRatio * sinTheta > 1.0;
        vec3 refractionDirection;
        if(cantRefract || reflectance(cosTheta, refractionRatio) > randomDouble(sharedRandomDevice)){
            refractionDirection = reflect(rayIncoming.direction(), record.normal);
        }
        else{
            refractionDirection = refract(rayIncoming.direction(), record.normal, refractionRatio);
        }
        rayScattered = ray(record.hitLocation, refractionDirection);

        return true;
    }

    virtual color getAlbedo() const override{
        return albedo;
    }
};

}; //end namespace mat

#endif //MATERIAL_HPP