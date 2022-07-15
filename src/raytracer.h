#ifndef RAYTRACEREXPERIMENT_RAYTRACER_H
#define RAYTRACEREXPERIMENT_RAYTRACER_H

#include <Eigen>
#include "scene.h"

#define RAY_TRACING_THRESHOLD 0.000001

using namespace Eigen;

class Ray {
public:
    Ray(Vector3f &o, Vector3f &d) : o(o), d(d){}
    Vector3f o, d;
};

bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance);


#endif //RAYTRACEREXPERIMENT_RAYTRACER_H
