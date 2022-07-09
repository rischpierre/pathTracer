
#include <iostream>
#include <fstream>
#include <Eigen>

#include "scene.h"
#include "raytracer.h"
using namespace Eigen;

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