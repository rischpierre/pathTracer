#ifndef PATHTRACER_GLOBALILUMINTEGRATOR_H
#define PATHTRACER_GLOBALILUMINTEGRATOR_H

#include <Eigen>

#include "accelerator.h"
#include "raytracer.h"
#include "renderSettings.h"
#include "scene.h"

// Represents the point shaded, the point on the surface that is hit by a ray
struct ShadingPoint {
    const Eigen::Vector3f hitPoint, n;
    const Face face;
    const float u, v;
    const Shader shader;
};

// Main algorithm for computing the light contribution of a given ray
class Integrator {
  public:
    explicit Integrator(const Scene &scene, const Accelerator &accelerator) : scene(scene), accelerator(accelerator){};

    Scene scene;
    Accelerator accelerator;
    
    // Compute the color returned by a given ray
    // It is a recursive function that is called n times the indirect lighting depth 
    Eigen::Vector3f castRay(const Ray &ray, const Scene &scene, uint depth = 0);
    
    // Creates a sample arround an hemishpere on world space
    // This will then be moved on the shading point normal space
    Eigen::Vector3f createHemisphereSample(const float &r1, const float &r2);
    
    // Compute the direct lighting contribution from a given shading point
    // It traces a ray from the shading point to the light source and checks if it is occluded
    Eigen::Vector3f getDirectContribution(const Ray &ray, const Scene &scene, const ShadingPoint &shadingPoint);
    
    // Generates a shading point from a given ray
    static ShadingPoint computeShadingPoint(float u, float v, const Face &face, const Eigen::Vector3f &hitPoint,
                                            const Scene &scene);
};
#endif
