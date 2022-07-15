
#include <iostream>
#include <fstream>
#include <Eigen>

#include "scene.h"
#include "raytracer.h"
#include "math.h"
#include <ctime>

using namespace Eigen;


#define BG_COLOR_R 0.1f
#define BG_COLOR_G 0.1f
#define BG_COLOR_B 0.11f

#define MAIN_RESOLUTION_W 1280
#define MAIN_RESOLUTION_H 720
#define RESOLUTION_DIVIDER 2 // should be a multiple of 2 or 1

#define RESOLUTION_W (MAIN_RESOLUTION_W / RESOLUTION_DIVIDER)
#define RESOLUTION_H (MAIN_RESOLUTION_H / RESOLUTION_DIVIDER)

#define CAM_FILM_SIZE_W 3.6f   // 24 36 film back size
#define CAM_FILM_SIZE_H 2.25f  // !6:9 ratio
#define CAM_FOCAL_LENGTH 4  // in mmm
#define WORLD_MAX_DISTANCE 9999999.f
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
    std::cout << "Parsed scene in " << (t2 - t1) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;

    // todo continue main loop here

    t1 = clock();

    auto *pixels = new Vector3f[RESOLUTION_W * RESOLUTION_H];
    auto camOrig = Vector3f(0, 0,0);
    auto camDir = Vector3f(0, CAM_FOCAL_LENGTH, 0);
    auto ray = Ray(camOrig, camDir);

    for (int y = 0; y < RESOLUTION_H; y++) {
        for (int x = 0; x < RESOLUTION_W; x++) {

            ray.d[0] = lerpRange((float)x, 0, (float)RESOLUTION_W, -CAM_FILM_SIZE_W / 2,
                                         CAM_FILM_SIZE_W / 2);

            ray.d[2] = lerpRange((float)y, 0, (float)RESOLUTION_H, -CAM_FILM_SIZE_H / 2,
                                         CAM_FILM_SIZE_H / 2);

            float distance = WORLD_MAX_DISTANCE;
            float maxDistance = WORLD_MAX_DISTANCE;
            Face *nearestFace = nullptr;

            for (Mesh &mesh: scene.meshes){
                for (auto  &face: mesh.faces) {

                    bool intersected = isRayIntersectsTriangle(&ray, &face, &distance);
                    if (!intersected) {
                        continue;
                    }

                    if (distance < maxDistance) {
                        maxDistance = distance;
                        nearestFace = &face;
                    }
                }
                auto color = Vector3f(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);

                if (nearestFace) {
                    color[0] = 1;
                    color[1] = 0;
                    color[2] = 0;
                }
                pixels[y * RESOLUTION_W + x] = color;
            }
        }
    }

    t2 = clock();
    std::cout << "Rendered in " << (t2 - t1) / (double)CLOCKS_PER_SEC << " seconds" << std::endl;

    writeToFile("test.ppm", RESOLUTION_W, RESOLUTION_H, pixels);
    delete[] pixels;

    return 0;
}