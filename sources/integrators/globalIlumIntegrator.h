#ifndef PATHTRACER_GLOBALILUMINTEGRATOR_H
#define PATHTRACER_GLOBALILUMINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

class GlobalIlumIntegrator: public BaseIntegrator {
public:
    explicit GlobalIlumIntegrator(const Scene &scene, const Accelerator& accelerator) : BaseIntegrator(scene, accelerator) {};

    Eigen::Vector3f getColor(const Ray &ray, const Scene &scene);

    void
    computeShadingPoint(const Eigen::Vector2f &uv, const Face &face, const Eigen::Vector3f &hitPoint, Eigen::Vector3f &rSmoothNormal);

};
#endif
