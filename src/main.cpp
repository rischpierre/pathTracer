
#include <iostream>
#include <fstream>
#include <Eigen>

#include "scene.h"
#include "raytracer.h"
using namespace Eigen;


// todo put that somewhere else
void writeToFile(const std::string &path, int width, int height, const Vector3f *pixels) {

    FILE *f = fopen(path.c_str(), "w");
    if (!f) {
        std::cout << "Error opening file!" << std::endl;
        exit(1);
    }

    fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);

    for (int i = 0; i < width * height; i++) {
        int r = pixels[i](0) * 255;
        int g = pixels[i](1) * 255;
        int b = pixels[i](2) * 255;
        fprintf(f, "%d %d %d\n", r, g, b);
    }
    std::cout << "Wrote file " << path << std::endl;

    // test to see if the file is correct
//    int width = 2;
//    int height = 2;
//
//    auto *pixels = new Vector3f[width * height];
//    pixels[0] = Vector3f(1, 0, 0);
//    pixels[1] = Vector3f(1, 0, 0);
//    pixels[2] = Vector3f(0, 1, 0);
//    pixels[3] = Vector3f(0, 1, 0);
//
//    writeToFile("test.ppm", width, height, pixels);
//
//    delete[] pixels;
}

int main(int argc, char *argv[]){

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <path to usd scene>" << std::endl;
        return 1;
    }

    std::ifstream file;
    std::string path = argv[1];
    file.open(path);
    if (!file){
        std::cout << "Error opening file" << path << std::endl;
        return 1;
    }

    Scene scene = Scene(path);

    return 0;
}