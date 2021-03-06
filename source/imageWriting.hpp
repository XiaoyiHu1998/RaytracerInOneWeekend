#ifndef IMAGE_WRITING_HPP
#define IMAGE_WRITING_HPP

#include "vec3.hpp"


inline void writeColorPPM(std::ostream& out, const color& pixelColorSum, int samplesPerPixel){
    double divider = 1.0/samplesPerPixel;

    double r = pixelColorSum.x() * divider;
    double g = pixelColorSum.y() * divider;
    double b = pixelColorSum.z() * divider;

    uint8_t r_byte = static_cast<int>(256 * clamp(r, 0, 0.999));
    uint8_t g_byte = static_cast<int>(256 * clamp(g, 0, 0.999));
    uint8_t b_byte = static_cast<int>(256 * clamp(b, 0, 0.999));

    out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

inline void writeColorPNG(uint8_t* imageData, int index, const color& pixelColorSum, int samplesPerPixel){
    double divider = 1.0/samplesPerPixel;

    double r = pixelColorSum.x() * divider;
    double g = pixelColorSum.y() * divider;
    double b = pixelColorSum.z() * divider;

    uint8_t r_byte = static_cast<int>(256 * clamp(r, 0, 0.999));
    uint8_t g_byte = static_cast<int>(256 * clamp(g, 0, 0.999));
    uint8_t b_byte = static_cast<int>(256 * clamp(b, 0, 0.999));
    uint8_t a_byte = 255;

    imageData[index++] = r_byte;
    imageData[index++] = g_byte;
    imageData[index++] = b_byte;
    imageData[index++] = a_byte;
}

inline void writeColor(std::ostream& out, std::vector<uint8_t>& imageData, const int index, const color& pixelColorSum, int samplesPerPixel){
    double divider = 1.0/samplesPerPixel;

    float r = static_cast<float>(std::sqrt(pixelColorSum.x() * divider));
    float g = static_cast<float>(std::sqrt(pixelColorSum.y() * divider));
    float b = static_cast<float>(std::sqrt(pixelColorSum.z() * divider));

    uint8_t r_byte = static_cast<int>(256 * clamp(r, 0, 0.999));
    uint8_t g_byte = static_cast<int>(256 * clamp(g, 0, 0.999));
    uint8_t b_byte = static_cast<int>(256 * clamp(b, 0, 0.999));
    uint8_t a_byte = 255;

    #ifdef PPM_OUTPUT
        out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
    #endif

    #ifdef PNG_OUTPUT
        imageData[index + 0] = r_byte;
        imageData[index + 1] = g_byte;
        imageData[index + 2] = b_byte;
        // imageData[index + 3] = a_byte;
    #endif

}

inline void convertSDRtoHDR(const std::vector<uint8_t>& sdrBuffer, std::vector<float>& hdrBuffer){
    for(int i = 0; i < hdrBuffer.size(); i++){
        hdrBuffer[i] = static_cast<float>(sdrBuffer[i] / 255.999);
    }
}

inline void convertHDRtoSDR(const std::vector<float>& hdrBuffer, std::vector<uint8_t>& sdrBuffer){
    for(int i = 0; i < hdrBuffer.size(); i++){
        sdrBuffer[i] = static_cast<uint8_t>(255.999 * hdrBuffer[i]);
    }
}

#endif //IMAGE_WRITING_HPP