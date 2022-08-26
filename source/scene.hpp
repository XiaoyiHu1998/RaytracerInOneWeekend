#ifndef SCENE_HPP
#define SCENE_HPP

#include "rtweekend.hpp"
#include "material.hpp"
#include "hittableList.hpp"
#include "rectangle.hpp"

enum class scene {
    randomBalls,
    twoCheckeredSpheres,
    twoPerlinSpheres,
    earth,
    spaceEarth,
    cornellBox
};

hittableList randomScene() {
    hittableList world;

    auto groundTexture = std::make_shared<checkerTexture>(color(0.15, 0.15, 0.15), color(0.95, 0.85, 0.85));
    world.add(std::make_shared<sphere>(point3(0,-1000,0), point3(0,-1000,0), 1000, 0.0, 1.0, std::make_shared<mat::lambertian>(groundTexture)));


    for (int a = -12; a < 12; a++) {
        for (int b = -12; b < 12; b++) {
            auto choose_mat = randomFloat(sharedRng);
            point3 startCenter(a + 0.9*randomFloat(sharedRng), 0.2, b + 0.9*randomFloat(sharedRng));
            point3 endCenter = startCenter + point3(0, randomFloat(sharedRng, 0, 0.), 0);

            if ((startCenter - point3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    color albedo = randomVec3() * randomVec3();
                    sphere_material = std::make_shared<mat::lambertian>(albedo);
                    world.add(std::make_shared<sphere>(startCenter, endCenter, 0.2, 0, 1.0, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    color albedo = randomVec3(0.5, 1.0);
                    auto fuzz = randomFloat(sharedRng, 0, 0.5);
                    sphere_material = std::make_shared<mat::metal>(albedo, fuzz);
                    world.add(std::make_shared<sphere>(startCenter, endCenter, 0.2, 0, 1.0, sphere_material));
                } else {
                    // glass
                    sphere_material = std::make_shared<mat::dielectric>(1.5);
                    world.add(std::make_shared<sphere>(startCenter, endCenter, 0.2, 0, 1.0, sphere_material));
                }
            }
        }
    }
    
    std::shared_ptr<imageTexture> sunTexture = std::make_shared<imageTexture>("./../images/sunTexture.jpg");
    std::shared_ptr<mat::diffuseLight> sunMaterial = std::make_shared<mat::diffuseLight>(sunTexture, 0.9);

    for (int a = -12; a < 12; a++) {
        for (int b = -12; b < 12; b++) {
            float size = randomFloat(sharedRng, 0.001, 0.03);
            float height = 2 + randomFloat(sharedRng, -0.65, 0.65);
            point3 startCenter(a + 0.9*randomFloat(sharedRng), height, b + 0.9*randomFloat(sharedRng));

            if ((startCenter - point3(4, 2, 0)).length() > 2.5) {
                    world.add(std::make_shared<sphere>(startCenter, size, sunMaterial));
            }
        }
    }

    auto material2 = std::make_shared<mat::lambertian>(std::make_shared<perlinTexture>());
    world.add(std::make_shared<sphere>(point3(-4, 1, 0), point3(-4, 1, 0), 1.0, 0.0, 1.0, material2));

    auto material1 = std::make_shared<mat::dielectric>(1.5);
    world.add(std::make_shared<sphere>(point3(0, 1, 0), point3(0, 1, 0), 1.0, 0.0, 1.0, material1));

    auto material3 = std::make_shared<mat::metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(4, 1, 0), point3(4, 1, 0), 1.0, 0.0, 1.0, material3));

    std::shared_ptr<sphere> sun = std::make_shared<sphere>(point3(1.5, 7, 0), 5.0, sunMaterial);
    world.add(sun);

    return world;
}

hittableList twoCheckeredSpheres(){
    hittableList world;

    auto checker = std::make_shared<checkerTexture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    auto checkeredMaterial = std::make_shared<mat::lambertian>(checker);

    glm::vec3 topSphereLocation = glm::vec3(0, 10, 0);
    glm::vec3 underSphereLocation = glm::vec3(0,-10, 0);

    world.add(std::make_shared<sphere>(topSphereLocation, topSphereLocation, 10, 0, 1, checkeredMaterial));
    world.add(std::make_shared<sphere>(underSphereLocation, underSphereLocation, 10, 0, 1, checkeredMaterial));

    return world;
}

hittableList twoPerlinSpheres(){
    hittableList world;

    std::shared_ptr<perlinTexture> noiseTexture = std::make_shared<perlinTexture>(4.0);
    std::shared_ptr<mat::lambertian> noiseMaterial = std::make_shared<mat::lambertian>(noiseTexture);

    world.add(std::make_shared<sphere>(point3(0,-1000,0), point3(0,-1000,0), 1000, 0, 1, noiseMaterial));
    world.add(std::make_shared<sphere>(point3(0, 2, 0), point3(0, 2, 0), 2, 0, 1, noiseMaterial));

    return world;
}

hittableList earth(){
    std::shared_ptr<imageTexture> earthTexture = std::make_shared<imageTexture>("./../images/earthmap.jpg");
    std::shared_ptr<mat::lambertian> earthMaterial = std::make_shared<mat::lambertian>(earthTexture);
    std::shared_ptr<sphere> earthGlobe = std::make_shared<sphere>(point3(0,0,0), 2, earthMaterial);

    return hittableList(earthGlobe);
}

hittableList spaceEarth(){
    hittableList world;

    std::shared_ptr<imageTexture> earthTexture = std::make_shared<imageTexture>("./../images/earthmap.jpg");
    std::shared_ptr<mat::lambertian> earthMaterial = std::make_shared<mat::lambertian>(earthTexture);
    std::shared_ptr<sphere> earth = std::make_shared<sphere>(point3(0,0,0), 2, earthMaterial);

    std::shared_ptr<imageTexture> moonTexture = std::make_shared<imageTexture>("./../images/moontexture.jpg");
    std::shared_ptr<mat::lambertian> moonMaterial = std::make_shared<mat::lambertian>(moonTexture);
    std::shared_ptr<sphere> moon = std::make_shared<sphere>(point3(3.0, -0.25, 2.5), 0.5, moonMaterial);

    std::shared_ptr<imageTexture> sunTexture = std::make_shared<imageTexture>("./../images/sunTexture.jpg");
    std::shared_ptr<mat::diffuseLight> sunMaterial = std::make_shared<mat::diffuseLight>(sunTexture, 1.0);
    std::shared_ptr<sphere> sun = std::make_shared<sphere>(point3(6, 2.5, -2), 2.5, sunMaterial);

    world.add(earth);
    world.add(moon);
    world.add(sun);

    return world;
}

hittableList cornellBox(){
    hittableList world;
    
    std::shared_ptr<mat::lambertian> red = std::make_shared<mat::lambertian>(color(1,0,0));
    std::shared_ptr<mat::lambertian> green = std::make_shared<mat::lambertian>(color(0,1,0));
    std::shared_ptr<mat::lambertian> white = std::make_shared<mat::lambertian>(color(1,1,1));
    std::shared_ptr<mat::diffuseLight> light = std::make_shared<mat::diffuseLight>(color(1,1,1), 4.0);

    world.add(std::make_shared<rectangleYZ>(0, 555, 0, 555, 555, green));
    world.add(std::make_shared<rectangleYZ>(0, 555, 0, 555, 0, red));
    world.add(std::make_shared<rectangleXZ>(100, 455, 100, 455, 554, light));
    world.add(std::make_shared<rectangleXZ>(0, 555, 0, 555, 0, white));
    world.add(std::make_shared<rectangleXZ>(0, 555, 0, 555, 555, white));
    world.add(std::make_shared<rectangleXY>(0, 555, 0, 555, 555, white));
    return world;
}

void setScene(scene sceneSelection, hittableList& world, point3& cameraPosition, point3& cameraTarget, point3& cameraUp, float& vFov, color& backgroundColor){
    switch(sceneSelection){
        case scene::randomBalls:
            world = randomScene();
            cameraPosition = point3(13,2,3);
            cameraTarget = point3(0,0,0);
            cameraUp = point3(0,1,0);
            backgroundColor = color(0.0025, 0.0025, 0.0025);
            break;

        case scene::twoCheckeredSpheres:
            world = twoCheckeredSpheres();
            cameraPosition = point3(13,2,3);
            cameraTarget = point3(0,0,0);
            cameraUp = point3(0,1,0);
            backgroundColor = color(0.70, 0.80, 1.00);
            break;

        case scene::twoPerlinSpheres:
            world = twoPerlinSpheres();
            cameraPosition = point3(13,2,3);
            cameraTarget = point3(0,0,0);
            cameraUp = point3(0,1,0);
            backgroundColor = color(0.70, 0.80, 1.00);
            break;

        case scene::earth:
            world = earth();
            cameraPosition = point3(13,2,3);
            cameraTarget = point3(0,0,0);
            cameraUp = point3(0,1,0);
            backgroundColor = color(0.70, 0.80, 1.00);
            break;

        case scene::spaceEarth:
            world = spaceEarth();
            cameraPosition = point3(13,2,3);
            cameraTarget = point3(0,0,0);
            cameraUp = point3(0,1,0);
            backgroundColor = color(0,0,0);
            break;

        case scene::cornellBox:
            world = cornellBox();
            cameraPosition = point3(278, 278, -800);
            cameraTarget = point3(278, 278, 0);
            cameraUp = point3(0,1,0);
            vFov = 40.0;
            backgroundColor = color(0,0,0);
            break;

        default:
            break;
    }
}

#endif //SCENE_HPP