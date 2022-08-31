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
    const Shader shader;
};


class GlobalIlumIntegrator: public BaseIntegrator {
public:
    explicit GlobalIlumIntegrator(const Scene &scene, const Accelerator& accelerator) : BaseIntegrator(scene, accelerator) {};

    Eigen::Vector3f castRay(const Ray &ray, const Scene &scene, uint depth=0);

    Eigen::Vector3f createHemisphereSample(const float& r1, const float& r2);

    Eigen::Vector3f getDirectContribution(
            const Ray &ray,
            const Scene &scene,
            const ShadingPoint& shadingPoint
    );

    static ShadingPoint computeShadingPoint(float u, float v, const Face &face, const Eigen::Vector3f &hitPoint, const Scene& scene);

private:
    uint indirectSamples = 4;
    uint indirectDepth = 3;

};
#endif
