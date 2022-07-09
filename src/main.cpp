
#include <iostream>
#include <fstream>
#include <Eigen>

#include "scene.h"

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

//    for (auto &point: *scene.getPoints()) {
//        for (int i = 0; i < 3; i++) {
//            std::cout << point[i] << std::endl;
//        }
//    }


    // test eigen
    Eigen::Vector2f v(1, 2);
    Eigen::Vector2f v1(4, 4);

    float x = v1.dot(v);

    Eigen::MatrixXd m(2,2);
    m(0,0) = 1;
    m(0,1) = 4;
    m(1,0) = 1;
    m(1,1) = 5;

    Eigen::MatrixXd m2(2,2);
    m2(0,0) = 5;
    m2(0,1) = 4;
    m2(1,0) = 5;
    m2(1,1) = 6;

    Eigen::MatrixXd m3 = m * m2;

    std::cout << m3 << std::endl;
    std::cout << v << std::endl;
    std::cout << x << std::endl;

    return 0;
}