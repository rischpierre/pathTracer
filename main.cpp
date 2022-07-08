#include "scene.h"

int main(int argc, char *argv[]){
    const std::string path = "../exampleUSD.usda";
    Scene scene = Scene(path);

    for (auto &point: *scene.getPoints()) {
        for (int i = 0; i < 3; i++) {
            std::cout << point[i] << std::endl;
        }
    }

    return 0;
}