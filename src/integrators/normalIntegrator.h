#ifndef PATHTRACER_NORMALINTEGRATOR_H
#define PATHTRACER_NORMALINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

class NormalIntegrator: public BaseIntegrator {
public:
    explicit NormalIntegrator(Scene &scn) : BaseIntegrator(scn){};

    Eigen::Vector3f getColor(const Ray &ray);

};



#endif //PATHTRACER_NORMALINTEGRATOR_H
