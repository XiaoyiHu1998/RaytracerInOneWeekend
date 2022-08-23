#ifndef TEXTURES_HPP
#define TEXTURES_HPP

#include "rtweekend.hpp"
#include "perlin.hpp"
#include "imageWriting.hpp"


class texture{
public:
    virtual color value(float u, float v, const glm::vec3& point) const = 0;
};

class solidColorTexture : public texture{
private:
    color colorValue;

public:
    solidColorTexture(){}

    solidColorTexture(const glm::vec3& color):
        colorValue{color}
    {}

    solidColorTexture(float red, float green, float blue):
        colorValue{glm::vec3(red, green, blue)}
    {}

    virtual color value(float u, float v, const glm::vec3& point) const override {
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

    virtual color value(float u, float v, const glm::vec3& point) const override {
        float sineSum = sin(point.x * 10) * sin(point.y * 10) * sin(point.z * 10);
        return sineSum < 0 ? odd->value(u, v, point) : even->value(u, v, point);
    }
};

class perlinTexture : public texture{
private:
    perlin perlinNoise;
    float scale;
public:
    perlinTexture(){}
    perlinTexture(float scale):
        scale{scale}
        {}

    virtual color value(float u, float v, const glm::vec3& point) const override {
        return color(1,1,1) * 0.5f * (1.0f + sin(scale * point.z + 10 * perlinNoise.turbulence(point)));
    }
};

class imageTexture : public texture{
private:
    unsigned char* imageData;
    int width;
    int height;
    int bytesPerPixel;
public:
    imageTexture():
        imageData{nullptr},
        width{0},
        height{0},
        bytesPerPixel{0}
        {}

    imageTexture(const char* imagePath){
        imageData = stbi_load(imagePath, &width, &height, &bytesPerPixel, 3);

        if(!imageData){
            std::cerr << "ERROR: failed to load image at " << imagePath << ".\n" << std::flush;
            width = 0;
            height = 0;
        }
    }

    ~imageTexture(){
        stbi_image_free(imageData);
    }

    virtual color value(float u, float v, const glm::vec3& point) const override{
        if(imageData == nullptr)
            return color(1,0,1);

        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);

        int pixelX = static_cast<int>(u * width);
        int pixelY = static_cast<int>(v * height);

        if(pixelX >= width)  pixelX = width - 1;
        if(pixelY >= height) pixelY = height - 1;

        int pixelIndex = pixelY * width * bytesPerPixel + pixelX * bytesPerPixel;
        double r = imageData[pixelIndex]     / 255.0;
        double g = imageData[pixelIndex + 1] / 255.0;
        double b = imageData[pixelIndex + 2] / 255.0;

        return color(r, g, b);
    }
};

#endif //TEXTURES_HPP