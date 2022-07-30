#include "debugAccIntegrator.h"


Eigen::Vector3f DebugAccIntegrator::getColor(const Ray &ray, const Scene &scene, const Accelerator& accelerator) {

    auto color = Eigen::Vector3f(0.f, 0.f, 0.f);

    const BVHNode* nearestNode= nullptr;
    float minRayBoxDistance = std::numeric_limits<float>::max();

    std::vector<BVHNode> intersectedNodes = accelerator.getIntersectedNodes(ray);

    for (const BVHNode& node : intersectedNodes) {
        float rayBoxDistance = (node.bbox.center() - ray.o).norm();

        if (rayBoxDistance < minRayBoxDistance) {
            minRayBoxDistance = rayBoxDistance;
            nearestNode= &node;
        }
    }

    if (nearestNode){
        color[nearestNode->id % 3] = 1;

    }
    return color;

}