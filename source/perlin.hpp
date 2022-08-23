#ifndef PERLIN_HPP
#define PERLIN_HPP

#include "rtweekend.hpp"

class perlin{
private:
    static const int pointCount = 256;
    glm::vec3* randomVec3Array;
    int* permX;
    int* permY;
    int* permZ;

    static int* perlinGeneratePerm(){
        auto p = new int[pointCount];
        
        for(int i = 0; i < perlin::pointCount; i++){
            p[i] = i;
        }
        
        permute(p, pointCount);

        return p;
    }

    static void permute(int* p, int n){
        for(int i = n - 1; i > 0; i--){
            int target = randomInt(sharedRng, 0, i + 1);
            int temp = p[i];
            p[i] = p[target];
            p[target] = temp;
        }
    }

    static float trilinearInterpolation(glm::vec3 c[2][2][2], float u, float v, float w){
        auto accum = 0.0;

        auto hermeticRounding = [](float value){ return value * value * (3 - 2* value); };
        float uu = hermeticRounding(u);
        float vv = hermeticRounding(v);
        float ww = hermeticRounding(w);

        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                for(int k = 0; k < 2; k++){
                    glm::vec3 weightVector(u-i, v-j, w-k);
                    accum += (i * uu + (1 - i) * (1 - uu)) * 
                             (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) *
                             dot(c[i][j][k], weightVector);
                }
            }
        }

        return accum;
    }

public:
    perlin(){
        randomVec3Array = new glm::vec3[pointCount];
        for(int i = 0; i < pointCount; i++){
            randomVec3Array[i] = randomUnitVector();
        }

        permX = perlinGeneratePerm();
        permY = perlinGeneratePerm();
        permZ = perlinGeneratePerm();
    }

    ~perlin(){
        delete[] randomVec3Array;
        delete[] permX;
        delete[] permY;
        delete[] permZ;
    }

    float noise(const point3& point) const {
        auto removeFloor = [](float value){ return value - floor(value); };
        float u = removeFloor(point.x);
        float v = removeFloor(point.y);
        float w = removeFloor(point.z);
        
        auto floorFloatToInt = [](float value){ return static_cast<int>(floor(value)); };
        auto i = floorFloatToInt(point.x);
        auto j = floorFloatToInt(point.y);
        auto k = floorFloatToInt(point.z);

        glm::vec3 c[2][2][2];

        for(int di = 0; di < 2; di++){
            for(int dj = 0; dj < 2; dj++){
                for(int dk = 0; dk < 2; dk++){
                    c[di][dj][dk] = randomVec3Array[permX[(i + di) & 255] ^ permY[(j + dj) & 255] ^ permZ[(k + dk) & 255]];
                }
            }
        }


        return trilinearInterpolation(c, u, v, w);
    }

    float turbulence(const point3& point, int depth = 7) const {
        point3 tempPoint = point;
        float accum = 0.0;
        float weight = 1.0;

        for(int i = 0; i < depth; i++){
            accum += weight * noise(tempPoint);
            weight *= 0.5;
            tempPoint *= 2;
        }

        return fabs(accum);
    }

};



#endif //PERLIN_HPP