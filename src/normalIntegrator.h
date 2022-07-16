#ifndef PATHTRACER_NORMALINTEGRATOR_H
#define PATHTRACER_NORMALINTEGRATOR_H

#define WORLD_MAX_DISTANCE 9999999.f
#define BG_COLOR_R 0.1f
#define BG_COLOR_G 0.1f
#define BG_COLOR_B 0.1f

#include <Eigen>

#include "scene.h"
#include "raytracer.h"

class NormalIntegrator {
public:
    explicit NormalIntegrator(const Scene &scn) : scene(scn){};

    Eigen::Vector3f getColor(const Ray &ray);

private:
    Scene scene;
};



#endif //PATHTRACER_NORMALINTEGRATOR_H
