#ifndef PATHTRACER_GLOBALILUMINTEGRATOR_H
#define PATHTRACER_GLOBALILUMINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

struct ShadingPoint{
    const Eigen::Vector3f hitPoint;
    const Eigen::Vector3f n;
    const Face face;
    const float u;
    const float v;
};


class GlobalIlumIntegrator: public BaseIntegrator {
public:
    explicit GlobalIlumIntegrator(const Scene &scene, const Accelerator& accelerator) : BaseIntegrator(scene, accelerator) {};

    Eigen::Vector3f getColor(const Ray &ray, const Scene &scene);

    Eigen::Vector3f getDirectContribution(
            const Ray &ray,
            const Scene &scene,
            const ShadingPoint& shadingPoint
    );

    static ShadingPoint computeShadingPoint(float u, float v, const Face &face, const Eigen::Vector3f &hitPoint);

};
#endif
