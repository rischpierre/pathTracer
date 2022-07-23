#ifndef PATHTRACER_NORMALINTEGRATOR_H
#define PATHTRACER_NORMALINTEGRATOR_H

#include <Eigen>

#include "../raytracer.h"
#include "baseIntegrator.h"

class NormalIntegrator: public BaseIntegrator {
public:

    Eigen::Vector3f getColor(const Ray &ray, const Scene &scene, bool debug);

};



#endif //PATHTRACER_NORMALINTEGRATOR_H
