#ifndef PATHTRACER_DEBUGACCINTEGRATOR_H
#define PATHTRACER_DEBUGACCINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

class DebugAccIntegrator: public BaseIntegrator {
public:
    explicit DebugAccIntegrator(const Scene &scene, const Accelerator& accelerator) : BaseIntegrator(scene, accelerator) {};

    Eigen::Vector3f getColor(const Ray &ray, const Scene &scene);

};
#endif
