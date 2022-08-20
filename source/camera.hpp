#ifndef CAMERA_HPP
#define CAMERA_HPP

class camera{
private:
    point3 origin;
    vec3 horizontal;
    vec3 vertical;
    vec3 lowerLeftCorner;

    vec3 w;
    vec3 u;
    vec3 v;

    double lensRadius;

    double exposureStart;
    double exposureEnd;

public:
    camera(point3 cameraPosition, point3 cameraTarget, vec3 upDirection, 
    double verticalFOV, double aspectRatio, double aperture, double focusDistance,
    double startTime, double endTime){

        double theta = degrees_to_radians(verticalFOV);
        double viewportHeight = 2.0 * tan(theta / 2);
        double viewportWidth = aspectRatio * viewportHeight;
        double focalLength = 1.0;

        w = normalize(cameraPosition - cameraTarget);
        u = normalize(cross(upDirection, w));
        v = cross(w, u);

        origin = cameraPosition;
        horizontal = focusDistance * viewportWidth * u;
        vertical = focusDistance * viewportHeight * v;
        lowerLeftCorner = origin - horizontal/2 - vertical/2 - focusDistance * w;
        lensRadius = aperture / 2.0;

        exposureStart = startTime;
        exposureEnd = endTime;
    }

    camera(double viewportWidth, double viewportHeight, double focalLength, point3 cameraLocation = point3(0,0,0)){
        origin = cameraLocation;
        horizontal = vec3(viewportWidth, 0, 0);
        vertical = vec3(0, viewportHeight, 0);
        lowerLeftCorner = origin - horizontal/2 - vertical/2 - vec3(0, 0, focalLength);
    }

    ray getRay(double x, double y) const {
        vec3 lensPosition = lensRadius * randomInUnitDisk();
        vec3 lensPositionOffset = u * lensPosition.x() + v * lensPosition.y();

        return ray(origin + lensPositionOffset, lowerLeftCorner + x*horizontal + y*vertical - origin - lensPositionOffset, randomDouble(sharedRandomDevice, exposureStart, exposureEnd));
    }
};



#endif //CAMERA_HPP