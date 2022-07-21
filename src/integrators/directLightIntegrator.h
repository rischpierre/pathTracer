#ifndef PATHTRACER_DIRECTLIGHTINTEGRATOR_H
#define PATHTRACER_DIRECTLIGHTINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

class DirectLightIntegrator: public BaseIntegrator {
public:
    explicit DirectLightIntegrator(Scene &scn) : BaseIntegrator(scn) {};

    Eigen::Vector3f getColor(const Ray &ray, const Scene &scene);

    void
    computeShadingPoint(const Eigen::Vector2f &uv, const Face &face, const Eigen::Vector3f &hitPoint, Eigen::Vector3f &rSmoothNormal);

};
#endif //PATHTRACER_NORMALINTEGRATOR_H
