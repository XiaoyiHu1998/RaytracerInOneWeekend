#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include "rtweekend.hpp"

class texture{
public:
    virtual color value(double u, double v, const vec3& point) const = 0;
};

class solidColorTexture : public texture{
private:
    color colorValue;

public:
    solidColorTexture(){}

    solidColorTexture(const vec3& color):
        colorValue{color}
    {}

    solidColorTexture(double red, double green, double blue):
        colorValue{vec3(red, green, blue)}
    {}

    virtual color value(double u, double v, const vec3& point) const override {
        return colorValue;
    }
};

class checkerTexture : public texture{
private:
    std::shared_ptr<texture> even;
    std::shared_ptr<texture> odd;
public:
    checkerTexture(){}

    checkerTexture(const color& even, const color& odd):
        even{std::make_shared<solidColorTexture>(even)},
        odd{std::make_shared<solidColorTexture>(odd)}
    {}

    checkerTexture(const std::shared_ptr<texture>& even, const std::shared_ptr<texture>& odd):
        even{even},
        odd{odd}
    {}

    virtual color value(double u, double v, const vec3& point) const override {
        double sineSum = sin(point.x() * 10) * sin(point.y() * 10) * sin(point.z() * 10);
        return sineSum < 0 ? odd->value(u, v, point) : even->value(u, v, point);
    }
};

#endif //TEXTURES_HPP