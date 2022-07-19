#ifndef RAYTRACEREXPERIMENT_RAYTRACER_H
#define RAYTRACEREXPERIMENT_RAYTRACER_H

#include <Eigen>
#include "scene.h"

#define RAY_TRACING_THRESHOLD 0.000001


class Ray {
public:
    Ray(Eigen::Vector3f &o, Eigen::Vector3f &d) : o(o), d(d){}
    Eigen::Vector3f o, d;
};

bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance);


#endif //RAYTRACEREXPERIMENT_RAYTRACER_H
