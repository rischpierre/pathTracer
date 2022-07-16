#ifndef PATHTRACER_FACINGRATIOINTEGRATOR_H
#define PATHTRACER_FACINGRATIOINTEGRATOR_H


#include <Eigen>

#include "../scene.h"
#include "../raytracer.h"
#include "baseIntegrator.h"

class FacingRatioIntegrator: BaseIntegrator {
public:
    explicit FacingRatioIntegrator(Scene &scn) : BaseIntegrator(scn){};

    Eigen::Vector3f getColor(const Ray &ray);

};




#endif //PATHTRACER_FACINGRATIOINTEGRATOR_H
