#ifndef PERLIN_HPP
#define PERLIN_HPP

#include "rtweekend.hpp"

class perlin{
private:
    static const int pointCount = 256;
    double* randomFloat;
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
            int target = randomInt(sharedRandomDevice, 0, i + 1);
            int temp = p[i];
            p[i] = p[target];
            p[target] = temp;
        }
    }

    static double trilinearInterpolation(double c[2][2][2], double u, double v, double w){
        auto accum = 0.0;

        for(int i = 0; i < 2; i++){
            for(int j = 0; j < 2; j++){
                for(int k = 0; k < 2; k++){
                    accum += (i * u + (1 - i) * (1 - u)) * 
                             (j * v + (1 - j) * (1 - v)) *
                             (k * w + (1 - k) * (1 - w)) *
                             c[i][j][k];
                }
            }
        }

        return accum;
    }

public:
    perlin(){
        randomFloat = new double[pointCount];
        for(int i = 0; i < pointCount; i++){
            randomFloat[i] = randomDouble(sharedRandomDevice, 0, 1);
        }

        permX = perlinGeneratePerm();
        permY = perlinGeneratePerm();
        permZ = perlinGeneratePerm();
    }

    ~perlin(){
        delete[] randomFloat;
        delete[] permX;
        delete[] permY;
        delete[] permZ;
    }

    double noise(const point3& point) const {
        auto removeFloor = [](double value){ return value - floor(value); };
        double u = removeFloor(point.x());
        double v = removeFloor(point.y());
        double w = removeFloor(point.z());

        auto hermeticRounding = [](double value){ return value * value * (3 - 2* value); };
        u = hermeticRounding(u);
        v = hermeticRounding(v);
        w = hermeticRounding(w);
        
        auto floorDoubleToInt = [](double value){ return static_cast<int>(floor(value)); };
        auto i = floorDoubleToInt(point.x());
        auto j = floorDoubleToInt(point.y());
        auto k = floorDoubleToInt(point.z());

        double c[2][2][2];

        for(int di = 0; di < 2; di++){
            for(int dj = 0; dj < 2; dj++){
                for(int dk = 0; dk < 2; dk++){
                    c[di][dj][dk] = randomFloat[permX[(i + di) & 255] ^ permY[(j + dj) & 255] ^ permZ[(k + dk) & 255]];
                }
            }
        }


        return trilinearInterpolation(c, u, v, w);
    }

};



#endif //PERLIN_HPP