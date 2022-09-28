#ifndef RAYTRACEREXPERIMENT_RAYTRACER_H
#define RAYTRACEREXPERIMENT_RAYTRACER_H

#include <Eigen>
#include "scene.h"
#include "renderSettings.h"

class Ray {
public:
    Ray(Eigen::Vector3f &o, Eigen::Vector3f &d) : o(o), d(d){}
    Eigen::Vector3f o, d;
};

bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance, float &u, float &v);

bool isRayIntersectsBox(const Ray& ray, const BBox& bbox);


#endif //RAYTRACEREXPERIMENT_RAYTRACER_H
