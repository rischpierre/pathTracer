#include "debugAccIntegrator.h"


Eigen::Vector3f DebugAccIntegrator::getColor(const Ray &ray, const Scene &scene, const Accelerator& accelerator) {

    auto color = Eigen::Vector3f(0.f, 0.f, 0.f);

    const BBox* nearestBBox= nullptr;
    float minRayBoxDistance = std::numeric_limits<float>::max();

    std::vector<BBox> intersectedBBoxes = accelerator.getIntersectedBboxes(ray);

    for (const BBox& bbox : intersectedBBoxes) {
        float rayBoxDistance = (bbox.center() - ray.o).norm();

        if (rayBoxDistance < minRayBoxDistance) {
            minRayBoxDistance = rayBoxDistance;
            nearestBBox = &bbox;
        }
    }

    if (nearestBBox){
        color[0] = 1;
    }
    return color;

}