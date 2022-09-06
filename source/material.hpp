#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "rtweekend.hpp"
#include "hittable.hpp"
#include "texture.hpp"

class material{
public:
    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const = 0;
    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const = 0;
    virtual color emitted(const float u, const float v, const point3& hitLocation) const { return color(0,0,0); }
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
        glm::vec3 scatterDirection;

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
        if(nearZero(scatterDirection))
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
    float roughness;
public:
    metal(const color& albedo, const float roughness):
        albedo{albedo},
        roughness{roughness < 1.0f ? roughness : 1.0f}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override {
        glm::vec3 reflectedDirection = reflect(rayIncoming.direction(), record.normal) + roughness * randomInUnitSphere();
        rayScattered = ray(record.hitLocation, reflectedDirection, rayIncoming.hitTime());
        attenuation = albedo;
        return glm::dot(reflectedDirection, record.normal) > 0;
    }

    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const override{
        return albedo;
    }
};

class dielectric : public material{
private:
    color albedo;
    float refractionIndex;

    static float reflectance(float cosine, float refractionIndex){
        //schlicks approximation for reflectance
        float r0 = (1 - refractionIndex) / (1 + refractionIndex);
        r0 = std::pow(r0, 2);
        return r0 + (1 - r0) * std::pow((1 - cosine), 5);
    }
public:
    dielectric(float refractionIndex):
        refractionIndex{refractionIndex},
        albedo{color(1,1,1)}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override {
        attenuation = color(1.0, 1.0, 1.0);
        float refractionRatio = record.frontFace ? (1.0 / refractionIndex) : refractionIndex;

        float cosTheta = std::fmin(glm::dot(-glm::normalize(rayIncoming.direction()), record.normal), 1.0);
        float sinTheta = std::sqrt(1.0 - (cosTheta * cosTheta));

        bool cantRefract = refractionRatio * sinTheta > 1.0;
        glm::vec3 refractionDirection;
        if(cantRefract || reflectance(cosTheta, refractionRatio) > randomFloat(sharedRng)){
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
    float strength;
    float size;

public:
    diffuseLight(std::shared_ptr<texture> emmision, float strength = 1.0):
        emmision{emmision},
        strength{strength}
        {}
    
    diffuseLight(const color& emmisioncolor, float strength = 1.0):
        emmision{std::make_shared<solidColorTexture>(emmisioncolor)},
        strength{strength}
        {}

    virtual bool scatter(const ray& rayIncoming, const hitRecord& record, color& attenuation, ray& rayScattered) const override{
        return false;
    }

    virtual color emitted(const float u, const float v, const point3& hitLocation) const override {
        return strength * emmision->value(u, v, hitLocation);
    }

    virtual color getAlbedoColor(const ray& rayIncoming, const hitRecord& record, color& attenuation) const override {
        return emmision->value(record.u, record.v, record.hitLocation);
    }
};


class isotropic : public material{
private:
    std::shared_ptr<texture> albedo;

public:
    isotropic(std::shared_ptr<texture>& texture):
        albedo{texture}
    {}

    isotropic(color materialColor):
        albedo{std::make_shared<solidColorTexture>(materialColor)}
    {}

    virtual bool scatter(const ray& r, const hitRecord& record, color& attenuation, ray& rayScattered) const override {
        rayScattered = ray(record.hitLocation, randomInUnitSphere(), r.hitTime());
        attenuation = albedo->value(record.u, record.v, record.hitLocation);
        return true;
    }
};

}; //end namespace mat

#endif //MATERIAL_HPP