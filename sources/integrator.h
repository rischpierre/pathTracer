#ifndef PATHTRACER_GLOBALILUMINTEGRATOR_H
#define PATHTRACER_GLOBALILUMINTEGRATOR_H

#include <Eigen>

#include "scene.h"
#include "accelerator.h"
#include "raytracer.h"
#include "renderSettings.h"

struct ShadingPoint{
    const Eigen::Vector3f hitPoint;
    const Eigen::Vector3f n;
    const Face face;
    const float u;
    const float v;
    const Shader shader;
};


class Integrator{
public:
    explicit Integrator(const Scene &scene, const Accelerator& accelerator) : scene(scene), accelerator(accelerator) {};

    Scene scene;
    Accelerator accelerator;

    Eigen::Vector3f castRay(const Ray &ray, const Scene &scene, uint depth=0);

    Eigen::Vector3f createHemisphereSample(const float& r1, const float& r2);

    Eigen::Vector3f getDirectContribution(
            const Ray &ray,
            const Scene &scene,
            const ShadingPoint& shadingPoint
    );

    static ShadingPoint computeShadingPoint(float u, float v, const Face &face, const Eigen::Vector3f &hitPoint, const Scene& scene);

};
#endif
