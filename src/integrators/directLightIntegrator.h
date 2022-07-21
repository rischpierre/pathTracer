#ifndef PATHTRACER_DIRECTLIGHTINTEGRATOR_H
#define PATHTRACER_DIRECTLIGHTINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

class DirectLightIntegrator: public BaseIntegrator {
public:
    explicit DirectLightIntegrator(Scene &scn) : BaseIntegrator(scn){};

    Eigen::Vector3f getColor(const Ray &ray, const Scene &scene);

};

#endif //PATHTRACER_NORMALINTEGRATOR_H
