
#include <iostream>
#include <fstream>
#include <Eigen>
#include <ctime>
#include <tbb/tbb.h>
#include "helpers.h"

#include "scene.h"
#include "raytracer.h"
#include "integrators/normalIntegrator.h"
#include "integrators/facingRatioIntegrator.h"
#include "integrators/directLightIntegrator.h"
#include "accelerator.h"
#include "integrators/debugAccIntegrator.h"


#define MAIN_RESOLUTION_W 1280
#define MAIN_RESOLUTION_H 720
#define RESOLUTION_DIVIDER 4 // should be a multiple of 2 or 1
#define RESOLUTION_W (MAIN_RESOLUTION_W / RESOLUTION_DIVIDER)
#define RESOLUTION_H (MAIN_RESOLUTION_H / RESOLUTION_DIVIDER)
#define RAY_TRACING_THRESHOLD 0.000001


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
    clock_t t2 = clock();
    std::cout << "Parsed scene in " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;


    auto *pixels = new Eigen::Vector3f[RESOLUTION_W * RESOLUTION_H];

    t1 = clock();
    Accelerator accelerator;

    std::vector<Face> faces;
    for (auto &mesh : scene.meshes){
        for (auto &face : mesh.faces){
            faces.push_back(face);
        }
    }

    accelerator.build(faces);

    t2 = clock();
    std::cout << "Generating acceleration structure in " << (float)(t2 - t1) / CLOCKS_PER_SEC << " seconds" << std::endl;

    DirectLightIntegrator integratorDirect(scene, accelerator);
    DebugAccIntegrator integratorDebug(scene, accelerator);

    tbb::tick_count t3 = tbb::tick_count::now();
#ifdef SINGLE_THREADED
    std::cout << "Single threaded" << std::endl;

    for (int y = 0; y < RESOLUTION_H; y++) {
#else
    std::cout << "Multi threaded" << std::endl;
    tbb::parallel_for(0, RESOLUTION_H, [&](int y) {
#endif
        for (int x = 0; x < RESOLUTION_W; x++) {

            Ray ray = createCameraRay(scene.camera, x, y);

            Eigen::Vector3f colorDirect = integratorDirect.getColor(ray, scene);
            Eigen::Vector3f colorDebug = integratorDebug.getColor(ray, scene);

            pixels[y * RESOLUTION_W + x] = (5 * colorDebug/6 + colorDirect/6);
        }
    }
#ifndef SINGLE_THREADED
    );
#endif

    tbb::tick_count t4 = tbb::tick_count::now();
    std::cout << "Rendered in " << (t4 - t3).seconds() << " seconds" << std::endl;

    write_png_file(resultImageFile, RESOLUTION_W, RESOLUTION_H, pixels);
    std::cout << "Wrote file " << resultImageFile << std::endl;

    delete[] pixels;

    return 0;
}