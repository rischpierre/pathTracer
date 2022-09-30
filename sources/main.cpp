
#include <iostream>
#include <fstream>
#include <Eigen>
#include <ctime>
#include <tbb/tbb.h>
#include "helpers.h"

#include "scene.h"
#include "raytracer.h"
#include "accelerator.h"
#include "integrator.h"
#include "renderSettings.h"


// todo move in camera class
Ray createCameraRay(const Camera &cam, int x, int y){

   float sampleX = (cam.hAperture / (float)RESOLUTION_W) * (float)x - cam.hAperture / 2;
   // todo why - for the y sample ?
   float sampleY = - ((cam.vAperture / (float)RESOLUTION_H) * (float)y - cam.vAperture / 2);
   float sampleZ = -cam.focalLength;  // camera is pointing towards - z

   pxr::GfVec3f rayOrig = cam.toWorld.Transform(pxr::GfVec3f(0));
   pxr::GfVec3f rayDir = cam.toWorld.TransformDir(pxr::GfVec3f(sampleX, sampleY, sampleZ)).GetNormalized();

   Eigen::Vector3f rayD(rayDir[0], rayDir[1], rayDir[2]);
   Eigen::Vector3f rayO(rayOrig[0], rayOrig[1], rayOrig[2]);

   Ray ray(rayO, rayD);
   return ray;
}

int main(int argc, char *argv[]){

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <path to usd scene>" << std::endl;
        return 1;
    }
    std::string resultImageFile = "render.png";

    std::ifstream file;
    std::string path = argv[1];
    file.open(path);
    if (!file){
        std::cout << "Error opening file" << path << std::endl;
        return 1;
    }

    clock_t t1 = clock();
    Scene scene = Scene(path);
    scene.print();
    clock_t t2 = clock();
    std::cout << "Parsed scene in " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;

    Eigen::Vector3f pixels[RESOLUTION_W * RESOLUTION_H];

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
                int percentage = int((float)progress * 100 /(RESOLUTION_H * RESOLUTION_W));
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
            pixels[y * RESOLUTION_W + x] =  color;
            progress++;
        }
    }
#ifndef SINGLE_THREADED
    );
#endif

    tbb::tick_count t4 = tbb::tick_count::now();
    std::cout << "Rendered in " << (t4 - t3).seconds() << " seconds" << std::endl;

    write_png_file(resultImageFile, RESOLUTION_W, RESOLUTION_H, pixels);
    std::cout << "Wrote file " << resultImageFile << std::endl;

    return 0;
 }
