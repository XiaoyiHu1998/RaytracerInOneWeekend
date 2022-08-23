#ifndef CAMERA_HPP
#define CAMERA_HPP

class camera{
private:
    point3 origin;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 lowerLeftCorner;

    glm::vec3 w;
    glm::vec3 u;
    glm::vec3 v;

    float lensRadius;

    float exposureStart;
    float exposureEnd;

public:
    camera(point3 cameraPosition, point3 cameraTarget, glm::vec3 upDirection, 
    float verticalFOV, float aspectRatio, float aperture, float focusDistance,
    float startTime, float endTime){

        float theta = degrees_to_radians(verticalFOV);
        float viewportHeight = 2.0 * tan(theta / 2);
        float viewportWidth = aspectRatio * viewportHeight;
        float focalLength = 1.0;

        w = glm::normalize(cameraPosition - cameraTarget);
        u = glm::normalize(glm::cross(upDirection, w));
        v = glm::cross(w, u);

        origin = cameraPosition;
        horizontal = focusDistance * viewportWidth * u;
        vertical = focusDistance * viewportHeight * v;
        lowerLeftCorner = origin - horizontal * 0.5f - vertical * 0.5f - focusDistance * w;
        lensRadius = aperture / 2.0;

        exposureStart = startTime;
        exposureEnd = endTime;
    }

    camera(float viewportWidth, float viewportHeight, float focalLength, point3 cameraLocation = point3(0,0,0)){
        origin = cameraLocation;
        horizontal = glm::vec3(viewportWidth, 0, 0);
        vertical = glm::vec3(0, viewportHeight, 0);
        lowerLeftCorner = origin - horizontal * 0.5f - vertical * 0.5f - glm::vec3(0, 0, focalLength);
    }

    ray getRay(float x, float y, std::mt19937& rng) const {
        glm::vec3 lensPosition = lensRadius * randomInUnitDisk();
        glm::vec3 lensPositionOffset = u * lensPosition.x + v * lensPosition.y;

        return ray(origin + lensPositionOffset, lowerLeftCorner + x*horizontal + y*vertical - origin - lensPositionOffset, randomFloat(rng, exposureStart, exposureEnd));
    }
};



#endif //CAMERA_HPP