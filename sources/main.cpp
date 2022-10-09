
#include <Eigen>
#include <ctime>
#include <fstream>
#include <iostream>
#include <tbb/tbb.h>

#include "helpers.h"
#include "accelerator.h"
#include "integrator.h"
#include "raytracer.h"
#include "renderSettings.h"
#include "scene.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <path to usd scene>" << std::endl;
        return 1;
    }
    std::string resultImageFile = "render.png";

    std::ifstream file;
    std::string path = argv[1];
    file.open(path);
    if (!file) {
        std::cout << "Error opening file" << path << std::endl;
        return 1;
    }

    clock_t t1 = clock();
    Scene scene = Scene(path);
    scene.print();
    clock_t t2 = clock();
    std::cout << "Parsed scene in " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;

    // allocating on the head to avoid the limit on creating arrays
    Eigen::Vector3f *pixels = new Eigen::Vector3f[RESOLUTION_W * RESOLUTION_H];

    t1 = clock();

    Accelerator accelerator(scene);
    accelerator.build();

    t2 = clock();
    std::cout << "Build acceleration structure in " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds";

    std::cout << " (" << accelerator.getNodeNumber() << " nodes)" << std::endl;

    Integrator integrator(scene, accelerator);

    tbb::tick_count t3 = tbb::tick_count::now();
    int progress = 0;

#ifdef SINGLE_THREADED
    std::cout << "Single threaded" << std::endl;

    for (int y = 0; y < RESOLUTION_H; y++) {
#else
    std::cout << "Multi threaded" << std::endl;
    tbb::parallel_for(0, RESOLUTION_H, [&](int y) {
#endif
        for (int x = 0; x < RESOLUTION_W; x++) {

            Ray ray = createCameraRay(scene.camera, x, y);

            if (progress % ((RESOLUTION_H * RESOLUTION_W) / 10) == 0) { // every 10%
                int percentage = int((float)progress * 100 / (RESOLUTION_H * RESOLUTION_W));
                std::cout << percentage << " %" << std::endl;
            }
#if DEBUG_PIXEL == true
            Eigen::Vector3f color(0, 0, 0);
            if (x == DEBUG_PIXEL_X && y == DEBUG_PIXEL_Y) {
                std::cout << "debug pixel" << std::endl;
                Eigen::Vector3f color = integrator.castRay(ray, scene);
            }
#else
            Eigen::Vector3f color = integrator.castRay(ray, scene);
#endif
            pixels[y * RESOLUTION_W + x] = color;
            progress++;
        }
    }
#ifndef SINGLE_THREADED
    );
#endif

    tbb::tick_count t4 = tbb::tick_count::now();
    std::cout << "Rendered in " << (t4 - t3).seconds() << " seconds" << std::endl;

    writePngFile(resultImageFile, RESOLUTION_W, RESOLUTION_H, pixels);
    std::cout << "Wrote file " << resultImageFile << std::endl;

    delete[] pixels;

    return 0;
}
