#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "rtweekend.hpp"
#include "hittable.hpp"
#include "texture.hpp"

class material{
public:
    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const = 0;
    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const = 0;
    virtual color emitted(const double u, const double v, const point3& hitLocation) const { return color(0,0,0); }
};


namespace mat{
    
class lambertian : public material{
private:
    std::shared_ptr<texture> albedoTexture;
public:
    lambertian(const color& albedo):
        albedoTexture{std::make_shared<solidColorTexture>(albedo)}
        {}

    lambertian(const std::shared_ptr<texture>& albedo):
        albedoTexture{albedo}
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

        rayScattered = ray(record.hitLocation, scatterDirection, rayIncoming.hitTime());
        attenuation = albedoTexture->value(record.u, record.v, record.hitLocation);
        return true;
    }

    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const override{
        return albedoTexture->value(record.u, record.v, record.hitLocation);
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
        rayScattered = ray(record.hitLocation, reflectedDirection, rayIncoming.hitTime());
        attenuation = albedo;
        return dot(reflectedDirection, record.normal) > 0;
    }

    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const override{
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
        if(cantRefract || reflectance(cosTheta, refractionRatio) > randomDouble(sharedRng)){
            refractionDirection = reflect(rayIncoming.direction(), record.normal);
        }
        else{
            refractionDirection = refract(rayIncoming.direction(), record.normal, refractionRatio);
        }
        rayScattered = ray(record.hitLocation, refractionDirection, rayIncoming.hitTime());

        return true;
    }

    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const override{
        return albedo;
    }
};

class diffuseLight : public material{
private:
    std::shared_ptr<texture> emmision;
    double strength;
    double size;

public:
    diffuseLight(std::shared_ptr<texture> emmision, double strength = 1.0):
        emmision{emmision},
        strength{strength}
        {}
    
    diffuseLight(const color& emmisioncolor, double strength = 1.0):
        emmision{std::make_shared<solidColorTexture>(emmisioncolor)},
        strength{strength}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override{
        return false;
    }

    virtual color emitted(const double u, const double v, const point3& hitLocation) const override {
        return strength * emmision->value(u, v, hitLocation);
    }

    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const override {
        return emmision->value(record.u, record.v, record.hitLocation);
    }
};

}; //end namespace mat

#endif //MATERIAL_HPP