#define PNG_OUTPUT

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
//Single threaded rendering is borked at this moment!
#define MT

//enable BVH
#define BVH

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <chrono>
using namespace std::chrono_literals;

#include "rtweekend.hpp"
#include "material.hpp"
#include "sphere.hpp"
#include "hittableList.hpp"
#include "imageWriting.hpp"
#include "camera.hpp"
#include "workCounter.hpp"
#include "bvhNode.hpp"
#include "scene.hpp"

#ifdef OIDN
    #include "../vendor/OIDN/oidn.hpp"
#endif

color rayNormalColor(const ray& r, const hittable& world, int depth){
//object color[]
    hitRecord record;

    if(world.hit(r, 0.001, infinity, record)){
        return 0.5f * (record.normal + glm::vec3(1,1,1)); //normal vector color
    }
    
    return color(0,0,0);
}

color rayAlbedoColor(const ray& r, const color& backgroundColor, const hittable& world, int depth){
    //object color
    hitRecord record;

    color attenuation;
    if(world.hit(r, 0.001, infinity, record)){
        return record.materialPointer->getAlbedoColor(r, record, attenuation);
    }

    return backgroundColor;
}

color rayColor(const ray& r, const color& backgroundColor, const hittable& world, int depth){
    //return stop recursing at max depth
    if(depth <= 0){
        return color(0,0,0);
    }

    //object color
    hitRecord record;

    if(!world.hit(r, 0.001, infinity, record))
        return backgroundColor;

    ray rayScattered;
    color attenuation;
    color emmited = record.materialPointer->emitted(record.u, record.v, record.hitLocation);

    if(!record.materialPointer->scatter(r, record, attenuation, rayScattered))
        return emmited;

    //TODO: check remove emmited for optimization
    return emmited + attenuation * rayColor(rayScattered, backgroundColor, world, depth - 1);
}


void renderImage(const int image_width, const int image_height, const int pixelSampleCount, std::vector<uint8_t>& imageBuffer, 
                 const camera& worldCamera, const hittableList& world, workCounter& counter, const color& backgroundColor, const int maxDepth = 10)
{
    std::random_device randomDevice;
    std::mt19937 rng(randomDevice());
    int index = 0;

    for(int y = image_height - 1; y >= 0; y--){
        counter.incrementWorkMain();
        for(int x = 0; x < image_width; x++){
            color pixelColorSum = color(0,0,0);
            for(int s = 0; s < pixelSampleCount; s++){
                double u = (x + randomFloat(rng, 0.0, 1.0)) / (image_width - 1);
                double v = (y + randomFloat(rng, 0.0, 1.0)) / (image_height - 1);
                pixelColorSum += rayColor(worldCamera.getRay(u,v,rng), backgroundColor, world, maxDepth);
            }

            writeColor(std::cout, imageBuffer, index, pixelColorSum, pixelSampleCount);
            index += 3;
        }
    }
}

void renderAlbedo(const int image_width, const int image_height, const int pixelSampleCount, std::vector<uint8_t>& imageBuffer, 
                 const camera& worldCamera, const hittableList& world, workCounter& counter, const color& backgroundColor, const int maxDepth = 10)
{
    std::random_device randomDevice;
    std::mt19937 rng(randomDevice());
    int index = 0;

    for(int y = image_height - 1; y >= 0; y--){
        counter.incrementWorkAlbedo();
        for(int x = 0; x < image_width; x++){
            color pixelColorSum = color(0,0,0);
            for(int s = 0; s < pixelSampleCount; s++){
                double u = (x + randomFloat(rng, 0.0, 1.0)) / (image_width - 1);
                double v = (y + randomFloat(rng, 0.0, 1.0)) / (image_height - 1);
                pixelColorSum += rayAlbedoColor(worldCamera.getRay(u,v,rng), backgroundColor, world, maxDepth);
            }

            writeColor(std::cout, imageBuffer, index, pixelColorSum, pixelSampleCount);
            index += 3;
        }
    }
}

void renderNormal(const int image_width, const int image_height, const int pixelSampleCount, std::vector<uint8_t>& imageBuffer, 
                 const camera& worldCamera, const hittableList& world, workCounter& counter, const int maxDepth = 10)
{
    std::random_device randomDevice;
    std::mt19937 rng(randomDevice());
    int index = 0;

    for(int y = image_height - 1; y >= 0; y--){
        counter.incrementWorkNormal();
        for(int x = 0; x < image_width; x++){
            color pixelColorSum = color(0,0,0);
            for(int s = 0; s < pixelSampleCount; s++){
                double u = (x + randomFloat(rng, 0.0, 1.0)) / (image_width - 1);
                double v = (y + randomFloat(rng, 0.0, 1.0)) / (image_height - 1);
                pixelColorSum += rayNormalColor(worldCamera.getRay(u,v,rng), world, maxDepth);
            }

            writeColor(std::cout, imageBuffer, index, pixelColorSum, pixelSampleCount);
            index += 3;
        }
    }
}

int main(){
    // Image
    const double image_aspect_ratio = 3.0 / 2.0;
    const int image_width = 1000;
    const int image_height = static_cast<int>(image_width / image_aspect_ratio);
    const int pixelSampleCount = 36;
    const int maxDepth = 10;
    const int image_channels = 3;
    const int imageBufferSize = image_width * image_height * image_channels;
    
#pragma region buffersetup
    std::vector<uint8_t> inputSDR, albedoSDR, normalSDR, outputSDR;
    std::vector<float> inputHDR, outputHDR, albedoHDR, normalHDR;

    inputSDR.reserve(imageBufferSize);
    inputHDR.reserve(imageBufferSize);
    albedoSDR.reserve(imageBufferSize);
    albedoHDR.reserve(imageBufferSize);
    normalSDR.reserve(imageBufferSize);
    normalHDR.reserve(imageBufferSize);
    outputHDR.reserve(imageBufferSize);

    for(int i = 0; i < imageBufferSize; i++){
        inputHDR.push_back(0.0);
        inputSDR.push_back(0);
    }
    outputSDR = inputSDR;
    albedoSDR = inputSDR;
    normalSDR = inputSDR;
    outputHDR = inputHDR;
#pragma endregion

    //Scene
    hittableList world;
    point3 cameraPosition, cameraTarget, cameraUp;
    color backgroundColor(0,0,0);
    setScene(scene::randomBalls, world, cameraPosition, cameraTarget, cameraUp, backgroundColor);

    //Camera View
    auto focusDistance = 10.0;
    auto aperture = 0.1;
    camera worldCamera(cameraPosition, cameraTarget, cameraUp, 20.0, image_aspect_ratio, aperture, focusDistance, 0.0, 1.0);

    #ifdef BVH
    world = hittableList(std::make_shared<bvhNode>(world, 0.0, 1.0));
    #endif
    
    
    #if defined OIDN && !defined MT
        workCounter counter(image_height, 1);
        std::thread albedoThread(renderAlbedo, image_width, image_height, pixelSampleCount, std::ref(albedoSDR), std::ref(worldCamera), std::ref(world), std::ref(counter), 10);
        std::thread normalThread(renderNormal, image_width, image_height, pixelSampleCount, std::ref(normalSDR), std::ref(worldCamera), std::ref(world), std::ref(counter), 10);
    #endif
    
    #ifndef MT
        renderImage(image_width, image_height, pixelSampleCount, inputSDR, worldCamera, world, maxDepth);
    #endif

    #ifdef MT
        const int threadCount = std::max(1, static_cast<int>(std::thread::hardware_concurrency() - 1));
        const int samplesPerThread = std::max(1, pixelSampleCount / threadCount);

        std::vector<std::vector<uint8_t>> mainBuffers;
        std::vector<std::vector<uint8_t>> albedoBuffers;
        std::vector<std::vector<uint8_t>> normalBuffers;

        std::vector<std::thread> threadPoolMain;
        std::vector<std::thread> threadPoolAlbedo;
        std::vector<std::thread> threadPoolNormal;

        threadPoolMain.reserve(threadCount);
        threadPoolAlbedo.reserve(threadCount);
        threadPoolNormal.reserve(threadCount);

        workCounter counter(image_height, threadCount);

        for(int i = 0; i < threadCount; i++){
            mainBuffers.push_back(inputSDR);
        }
        albedoBuffers = mainBuffers;
        normalBuffers = mainBuffers;

        for(int i = 0; i < threadCount; i++){
            threadPoolMain.push_back(std::thread(renderImage, image_width, image_height, samplesPerThread, 
                                                 std::ref(mainBuffers[i]), std::ref(worldCamera), std::ref(world), 
                                                 std::ref(counter), std::ref(backgroundColor), 10));

            threadPoolAlbedo.push_back(std::thread(renderAlbedo, image_width, image_height, samplesPerThread, 
                                                   std::ref(albedoBuffers[i]), std::ref(worldCamera), std::ref(world), 
                                                   std::ref(counter), std::ref(backgroundColor), 10));

            threadPoolNormal.push_back(std::thread(renderNormal, image_width, image_height, samplesPerThread, 
                                                   std::ref(normalBuffers[i]), std::ref(worldCamera), std::ref(world), 
                                                   std::ref(counter), 10));
        }

        while(!counter.isWorkDone()){
            counter.outputWorkDone();
            std::this_thread::sleep_for(500ms);
        }
        counter.outputWorkDone();
        std::cerr << "\n" << std::flush;
        
        //clean up threadPool
        for (int i = 0; i < threadCount; i++){
            threadPoolMain[i].join();
            threadPoolAlbedo[i].join();
            threadPoolNormal[i].join();
        }

        std::cerr << '\r' << "merging thread imageBuffers" << std::flush;
        for(int i = 0; i < imageBufferSize; i++){
            int sumMain = 0;
            int sumAlbedo = 0;
            int sumNormal = 0;
            for(int j = 0; j < mainBuffers.size(); j++){
                sumMain += mainBuffers[j][i];
                sumAlbedo += albedoBuffers[j][i];
                sumNormal += normalBuffers[j][i];
            }
            inputSDR[i] = sumMain / threadCount;
            albedoSDR[i] = sumAlbedo / threadCount;
            normalSDR[i] = sumNormal / threadCount;
        }
    #endif

    #ifdef OIDN
        #if defined OIDN && !defined MT
        std::cerr << '\r' << "waiting for albedo and normal threads                                       " << std::flush;
        albedoThread.join();
        normalThread.join();
        std::cerr << '\r' << "albedo and normal renders done                                       " << std::flush;
        #endif

        // Create an Intel Open Image Denoise device
        std::cerr << '\r' << "creating devices                       " << std::flush;
        oidn::DeviceRef device = oidn::newDevice();
        device.commit();

        convertSDRtoHDR(inputSDR, inputHDR);
        convertSDRtoHDR(albedoSDR, albedoHDR);
        convertSDRtoHDR(normalSDR, normalHDR);

        // Create a filter for denoising a beauty (color) image using prefiltered auxiliary images too
        std::cerr << '\r' << "setting beautyfilter                                       " << std::flush;
        oidn::FilterRef filter = device.newFilter("RT"); // generic ray tracing filter
        filter.setImage("color",  &inputHDR[0],  oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // beauty
        filter.setImage("albedo", &albedoHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // auxiliary
        filter.setImage("normal", &normalHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // auxiliary
        filter.setImage("output", &outputHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width); // denoised beauty
        filter.set("hdr", true); // beauty image is HDR
        filter.set("cleanAux", false); // auxiliary images will be prefiltered
        filter.commit();

        // Create a separate filter for denoising an auxiliary albedo image (in-place)
        std::cerr << '\r' << "setting prefilter albedo                                       " << std::flush;
        oidn::FilterRef albedoFilter = device.newFilter("RT"); // same filter type as for beauty
        albedoFilter.setImage("albedo", &albedoHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        albedoFilter.setImage("output", &albedoHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        albedoFilter.commit();

        // Create a separate filter for denoising an auxiliary normal image (in-place)
        std::cerr << '\r' << "setting prefilter normal                                       " << std::flush;
        oidn::FilterRef normalFilter = device.newFilter("RT"); // same filter type as for beauty
        normalFilter.setImage("normal", &normalHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        normalFilter.setImage("output", &normalHDR[0], oidn::Format::Float3, image_width, image_height, 0, 0, 0 * image_width);
        normalFilter.commit();

        // Prefilter the auxiliary images
        std::cerr << '\r' << "prefiltering albedo                                       " << std::flush;
        albedoFilter.execute();
        std::cerr << '\r' << "prefiltering normals                                      " << std::flush;
        normalFilter.execute();

        // // Filter the beauty image
        std::cerr << '\r' << "denoising beauty image                                    " << std::flush;
        filter.execute();
        std::cerr << '\r' << "denoised beauty image                                     " << std::flush;

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
        convertHDRtoSDR(inputHDR, inputSDR);
    #endif

    #ifdef PNG_OUTPUT
        stbi_write_png("./../output/image.png", image_width, image_height, image_channels, &inputSDR[0], 0);
        #ifdef OIDN
            stbi_write_png("./../output/imageFiltered.png", image_width, image_height, image_channels, &outputSDR[0], 0);
            stbi_write_png("./../output/imageAlbedo.png", image_width, image_height, image_channels, &albedoSDR[0], 0);
            stbi_write_png("./../output/imageNormal.png", image_width, image_height, image_channels, &normalSDR[0], 0);
        #endif
    #endif


    std::cerr << "\rFinished.                                       " << std::flush;
    // std::cin.get();
}


//g++ source/main.cpp -I./source/ -I./vendor/ -lOpenImageDenoise -L./vendor/OIDN -o./build/raytracer.exe -O3