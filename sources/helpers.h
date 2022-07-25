#include <pxr/usd/usd/attribute.h>

float lerpRange(float x, float x0, float x1, float y0, float y1) {
    return ((y1 - y0) * x + x1 * y0 - x0 * y1) / (x1 - x0);
}


void printGfMat4f(const pxr::GfMatrix4d &input){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            std::cout << input[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printGfVec3f(const pxr::GfVec3f &input){
    for (int i = 0; i < 3; i++) {
        std::cout << input[i] << " ";
    }
    std::cout << std::endl;
}