#ifndef PATHTRACER_BASEINTEGRATOR_H
#define PATHTRACER_BASEINTEGRATOR_H


#define WORLD_MAX_DISTANCE 9999999.f
#define BG_COLOR_R 0.1f
#define BG_COLOR_G 0.1f
#define BG_COLOR_B 0.1f

#include "../scene.h"
#include "../accelerator.h"

class BaseIntegrator {
public:
    explicit BaseIntegrator(const Scene &scene, const Accelerator& accelerator) : scene(scene), accelerator(accelerator) {};

    Scene scene;
    Accelerator accelerator;
};


#endif //PATHTRACER_BASEINTEGRATOR_H
