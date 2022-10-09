#ifndef RAYTRACEREXPERIMENT_RAYTRACER_H
#define RAYTRACEREXPERIMENT_RAYTRACER_H

#include "renderSettings.h"
#include "scene.h"
#include <Eigen>

class Ray {
  public:
    Ray(Eigen::Vector3f o, Eigen::Vector3f d) : o(std::move(o)), d(std::move(d)) {}
    Eigen::Vector3f o, d;
};

// Check if a ray hits a given triangle. This is the implementation of the
// Möller-Trumbore algorithm
bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance, float &u, float &v);

// Check if a ray hits a given bounding box
bool isRayIntersectsBox(const Ray &ray, const BBox &bbox);

// Creates a ray starting from the camera focal point and intersecting the middle of the given pixel.
Ray createCameraRay(const Camera &cam, int x, int y);

#endif // RAYTRACEREXPERIMENT_RAYTRACER_H
