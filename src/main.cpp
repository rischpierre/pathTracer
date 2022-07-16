
#include <iostream>
#include <fstream>
#include <Eigen>
#include <ctime>
#include <tbb/tbb.h>
#include "mathHelper.h"

#include "scene.h"
#include "raytracer.h"
#include "normalIntegrator.h"

#define MAIN_RESOLUTION_W 1280
#define MAIN_RESOLUTION_H 720

#define RESOLUTION_DIVIDER 2 // should be a multiple of 2 or 1
#define RESOLUTION_W (MAIN_RESOLUTION_W / RESOLUTION_DIVIDER)

#define RESOLUTION_H (MAIN_RESOLUTION_H / RESOLUTION_DIVIDER)
#define CAM_FILM_SIZE_W 3.6f   // 24 36 film back size
#define CAM_FILM_SIZE_H 2.25f  // !6:9 ratio
#define CAM_FOCAL_LENGTH 4  // in mmm
#define RAY_TRACING_THRESHOLD 0.000001

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

    clock_t t1 = clock();
    Scene scene = Scene(path);
    clock_t t2 = clock();
    std::cout << "Parsed scene in " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;

    tbb::tick_count t3 = tbb::tick_count::now();

    auto *pixels = new Vector3f[RESOLUTION_W * RESOLUTION_H];
    auto camOrig = Vector3f(0, 0,0);
    auto camDir = Vector3f(0, CAM_FOCAL_LENGTH, 0);

    tbb::parallel_for(0, RESOLUTION_H, [&scene, &pixels, &camDir, &camOrig](int y) {

        for (int x = 0; x < RESOLUTION_W; x++) {

            auto ray = Ray(camOrig, camDir);
            ray.d[0] = lerpRange((float)x, 0, (float)RESOLUTION_W, -CAM_FILM_SIZE_W / 2,
                                         CAM_FILM_SIZE_W / 2);

            ray.d[2] = lerpRange((float)y, 0, (float)RESOLUTION_H, -CAM_FILM_SIZE_H / 2,
                                         CAM_FILM_SIZE_H / 2);

            NormalIntegrator integrator(scene);
            Eigen::Vector3f color = integrator.getColor(ray);

            pixels[y * RESOLUTION_W + x] = color;
        }
    });

    tbb::tick_count t4 = tbb::tick_count::now();
    std::cout << "Rendered in " << (t4 - t3).seconds() << " seconds" << std::endl;

    writeToFile("test.ppm", RESOLUTION_W, RESOLUTION_H, pixels);
    delete[] pixels;

    return 0;
}