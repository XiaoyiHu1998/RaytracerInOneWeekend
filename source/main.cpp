#define STB_IMAGE_WRITE_IMPLEMENTATION
#define PNG_OUTPUT
// #define PPM_OUTPUT

// Select diffusion mode
// #define IN_HEMISPHERE
// #define UNIT_SPHERE
#define UNIT_VECTOR

// Select render color mode
#define RENDER_MATERIALS
// #define RENDER_NORMALS

// Select Image Denoising
#define OIDN

//Select MultiThreading or Singlethreading
#define MT

#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include "stb_image_write.hpp"
#include "rtweekend.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "hittableList.hpp"
#include "imageWriting.hpp"
#include "camera.hpp"
#ifdef OIDN
    #include "vendor/OIDN/oidn.hpp"
#endif

color rayNormalColor(const ray& r, const hittable& world, int depth){
    //object color
    hitRecord record;

    if(world.hit(r, 0.001, infinity, record)){
        return 0.5 * (record.normal + vec3(1,1,1)); //normal vector color
    }
    
    return color(0,0,0);
}

color rayAlbedoColor(const ray& r, const hittable& world, int depth){
    //object color
    hitRecord record;

    if(world.hit(r, 0.001, infinity, record)){
        return record.materialPointer->getAlbedo();
    }

    vec3 normalizedDirection = normalize(r.direction());
    double dist = 0.5 * (normalizedDirection.y() + 1.0);
    return (1.0 - dist) * color(1.0, 1.0, 1.0) + dist * color(0.5, 0.7, 1.0);
}

color rayColor(const ray& r, const hittable& world, int depth){
    //return stop recursing at max depth
    if(depth <= 0){
        return color(0,0,0);
    }
    //object color
    hitRecord record;

    if(world.hit(r, 0.001, infinity, record)){
            ray rayScattered;
            color attenuation;
            if(record.materialPointer->scatter(r, record, attenuation, rayScattered))
                return attenuation * rayColor(rayScattered, world, depth - 1);
            
            return color(0,0,0);
    }

    //background color
    vec3 normalizedDirection = normalize(r.direction());
    double dist = 0.5 * (normalizedDirection.y() + 1.0);
    return (1.0 - dist) * color(1.0, 1.0, 1.0) + dist * color(0.5, 0.7, 1.0);
}

hittableList randomScene() {
    hittableList world;

    auto ground_material = std::make_shared<mat::lambertian>(color(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));


    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = randomDouble(sharedRandomDevice);
            point3 center(a + 0.9*randomDouble(sharedRandomDevice), 0.2, b + 0.9*randomDouble(sharedRandomDevice));

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = std::make_shared<mat::lambertian>(albedo);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = randomDouble(sharedRandomDevice, 0, 0.5);
                    sphere_material = std::make_shared<mat::metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<mat::dielectric>(1.5);
                    world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<mat::dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<mat::lambertian>(color(0.4, 0.2, 0.1));
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<mat::metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

void renderImage(const int image_width, const int image_height, const int pixelSampleCount, std::vector<uint8_t>& imageBuffer, const camera& worldCamera, const hittableList& world, const int maxDepth = 10){
    std::random_device randomDevice;
    int index = 0;

    for(int y = image_height - 1; y >= 0; y--){
        std::cerr << "\rRemaining Rows: " << y << "    " << std::flush;
        for(int x = 0; x < image_width; x++){
            color pixelColorSum = color(0,0,0);
            for(int s = 0; s < pixelSampleCount; s++){
                double u = (x + randomDouble(randomDevice, 0.0, 1.0)) / (image_width - 1);
                double v = (y + randomDouble(randomDevice, 0.0, 1.0)) / (image_height - 1);
                pixelColorSum += rayColor(worldCamera.getRay(u,v), world, maxDepth);
            }

            writeColor(std::cout, imageBuffer, index, pixelColorSum, pixelSampleCount);
            index += 3;
        }
    }
}

void renderAlbedo(const int image_width, const int image_height, const int pixelSampleCount, std::vector<uint8_t>& imageBuffer, const camera& worldCamera, const hittableList& world, const int maxDepth = 10){
    std::random_device randomDevice;
    int index = 0;

    for(int y = image_height - 1; y >= 0; y--){
        // std::cerr << "\rRemaining Rows: " << y << "    " << std::flush;
        for(int x = 0; x < image_width; x++){
            color pixelColorSum = color(0,0,0);
            for(int s = 0; s < pixelSampleCount; s++){
                double u = (x + randomDouble(randomDevice, 0.0, 1.0)) / (image_width - 1);
                double v = (y + randomDouble(randomDevice, 0.0, 1.0)) / (image_height - 1);
                pixelColorSum += rayAlbedoColor(worldCamera.getRay(u,v), world, maxDepth);
            }

            writeColor(std::cout, imageBuffer, index, pixelColorSum, pixelSampleCount);
            index += 3;
        }
    }
}

void renderNormal(const int image_width, const int image_height, const int pixelSampleCount, std::vector<uint8_t>& imageBuffer, const camera& worldCamera, const hittableList& world, const int maxDepth = 10){
    std::random_device randomDevice;
    int index = 0;

    for(int y = image_height - 1; y >= 0; y--){
        // std::cerr << "\rRemaining Rows: " << y << "    " << std::flush;
        for(int x = 0; x < image_width; x++){
            color pixelColorSum = color(0,0,0);
            for(int s = 0; s < pixelSampleCount; s++){
                double u = (x + randomDouble(randomDevice, 0.0, 1.0)) / (image_width - 1);
                double v = (y + randomDouble(randomDevice, 0.0, 1.0)) / (image_height - 1);
                pixelColorSum += rayNormalColor(worldCamera.getRay(u,v), world, maxDepth);
            }

            writeColor(std::cout, imageBuffer, index, pixelColorSum, pixelSampleCount);
            index += 3;
        }
    }
}

int main(){
    // Image
    const double image_aspect_ratio = 3.0 / 2.0;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / image_aspect_ratio);
    const int pixelSampleCount = 1;
    const int maxDepth = 10;
    const int image_channels = 3;
    const int imageBufferSize = image_width * image_height * image_channels;
    
    std::vector<uint8_t> inputSDR, albedoSDR, normalSDR, outputSDR;
    std::vector<float> inputHDR, outputHDR, albedoHDR, normalHDR;

    inputSDR.reserve(imageBufferSize);
    albedoSDR.reserve(imageBufferSize);
    normalSDR.reserve(imageBufferSize);
    inputHDR.reserve(imageBufferSize);
    outputHDR.reserve(imageBufferSize);
    albedoHDR.reserve(imageBufferSize);
    normalHDR.reserve(imageBufferSize);

    for(int i = 0; i < imageBufferSize; i++){
        inputHDR.push_back(0.0);
        inputSDR.push_back(0);
    }
    outputSDR = inputSDR;
    outputHDR = inputHDR;

    //World
    hittableList world = randomScene();


    //Camera
    point3 cameraPosition(13,2,3);
    point3 cameraTarget(0,0,0);
    vec3 cameraUp(0,1,0);
    auto focusDistance = 10.0;
    auto aperture = 0.1;

    camera worldCamera(cameraPosition, cameraTarget, cameraUp, 20.0, image_aspect_ratio, aperture, focusDistance);

    //Render
    #ifdef PPM_OUTPUT
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    #endif
    
    #ifdef OIDN
        std::thread albedoThread(renderAlbedo, image_width, image_height, 10, std::ref(albedoSDR), std::ref(worldCamera), std::ref(world), 10);
        std::thread normalThread(renderNormal, image_width, image_height, 10, std::ref(normalSDR), std::ref(worldCamera), std::ref(world), 10);
    #endif

    #ifdef MT
        const int threadCount = std::max(1, static_cast<int>(std::thread::hardware_concurrency() - 2));
        const int samplesPerThread = std::max(1, pixelSampleCount / threadCount);
        std::vector<std::vector<uint8_t>> imageBuffers;
        std::vector<std::thread> threadPool;
        threadPool.reserve(threadCount);

        for(int i = 0; i < threadCount; i++){
            imageBuffers.push_back(inputSDR);
        }

        for(int i = 0; i < threadCount; i++){
            threadPool.push_back(std::thread(renderImage, image_width, image_height, samplesPerThread, std::ref(imageBuffers[i]), std::ref(worldCamera), std::ref(world), 10));
        }

        for(int i = 0; i < threadCount; i++){
            threadPool[i].join();
        }

        std::cerr << '\r' << "merging thread imageBuffers" << std::flush;
        for(int i = 0; i < imageBufferSize; i++){
            int sum = 0;
            for(int j = 0; j < imageBuffers.size(); j++){
                sum += imageBuffers[j][i];
            }
            inputSDR[i] = sum / threadCount;
        }
    #endif
    
    #ifndef MT
        renderImage(image_width, image_height, pixelSampleCount, inputSDR, worldCamera, world, maxDepth);
    #endif

    #ifdef OIDN
        std::cerr << '\r' << "waiting for albedo and normal threads        " << std::flush;
        albedoThread.join();
        normalThread.join();
        std::cerr << '\r' << "albedo and normal renders done         " << std::flush;

        // Create an Intel Open Image Denoise device
        std::cerr << '\r' << "creating devices                       " << std::flush;
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();

        convertSDRtoHDR(inputSDR, inputHDR);
        convertSDRtoHDR(albedoSDR, albedoHDR);
        convertSDRtoHDR(normalSDR, normalHDR);

        // Create a filter for denoising a beauty (color) image using prefiltered auxiliary images too
        std::cerr << '\r' << "setting beautyfilter                   " << std::flush;
        oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
        filter.setImage("color",  &inputHDR[0],  oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // beauty
        filter.setImage("albedo", &albedoHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // auxiliary
        filter.setImage("normal", &normalHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // auxiliary
        filter.setImage("output", &outputHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // denoised beauty
        filter.set("hdr", true); // beauty image is HDR
        filter.set("cleanAux", false); // auxiliary images will be prefiltered
        filter.commit();

        // Create a separate filter for denoising an auxiliary albedo image (in-place)
        std::cerr << '\r' << "setting prefilter albedo                   " << std::flush;
        oidn::FilterRef albedoFilter = device.newFilter("RT"); // same filter type as for beauty
        albedoFilter.setImage("albedo", &albedoHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        albedoFilter.setImage("output", &albedoHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        albedoFilter.commit();

        // Create a separate filter for denoising an auxiliary normal image (in-place)
        std::cerr << '\r' << "setting prefilter normal                   " << std::flush;
        oidn::FilterRef normalFilter = device.newFilter("RT"); // same filter type as for beauty
        normalFilter.setImage("normal", &normalHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        normalFilter.setImage("output", &normalHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        normalFilter.commit();

        // Prefilter the auxiliary images
        std::cerr << '\r' << "prefiltering albedo             " << std::flush;
        albedoFilter.execute();
        std::cerr << '\r' << "prefiltering normals            " << std::flush;
        normalFilter.execute();

        // // Filter the beauty image
        std::cerr << '\r' << "denoising beauty image                     " << std::flush;
        filter.execute();
        std::cerr << '\r' << "denoised beauty image                      " << std::flush;

        // Check for errors
        const char* errorMessage;
        if (device.getError(errorMessage) != oidn::Error::None)
            std::cerr << "\nError: " << errorMessage << std::endl;
        
        convertHDRtoSDR(inputHDR, inputSDR);
        convertHDRtoSDR(outputHDR, outputSDR);
        convertHDRtoSDR(albedoHDR, albedoSDR);
        convertHDRtoSDR(normalHDR, normalSDR);
    #endif

    #ifndef OIDN
        convertHDRtoSDR(inputSDR, imageUnfilteredSDR);
    #endif

    #ifdef PNG_OUTPUT
        stbi_write_png("./../output/image.png", image_width, image_height, image_channels, &inputSDR[0], 0);
        #ifdef OIDN
            stbi_write_png("./../output/imageFiltered.png", image_width, image_height, image_channels, &outputSDR[0], 0);
            stbi_write_png("./../output/imageAlbedo.png", image_width, image_height, image_channels, &albedoSDR[0], 0);
            stbi_write_png("./../output/imageNormal.png", image_width, image_height, image_channels, &normalSDR[0], 0);
        #endif
    #endif
}


//g++ main.cpp -o ./../build/raytracer.exe -std=c++17 -O3 -L./vendor/OIDN/ -lOpenImageDenoise -ltbb12