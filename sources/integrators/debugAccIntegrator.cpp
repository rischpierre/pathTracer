#include "debugAccIntegrator.h"


Eigen::Vector3f DebugAccIntegrator::getColor(const Ray &ray, const Scene &scene) {

    auto color = Eigen::Vector3f(0.f, 0.f, 0.f);

    const Node* nearestNode = nullptr;
    float minRayBoxDistance = std::numeric_limits<float>::max();

    for (const Node* gn: accelerator.root.children){
        for (const Node* xn: gn->children) {
            for (const Node *n: xn->children) {
                if (!n)
                    continue;

                if (isRayIntersectsBox(ray, n->bbox)) {
                    float rayBoxDistance = (n->bbox.center() - ray.o).norm();

                    if (rayBoxDistance < minRayBoxDistance) {
                        minRayBoxDistance = rayBoxDistance;
                        nearestNode = n;
                    }
                }
            }
        }
    }
    if (nearestNode){
        color[nearestNode->id % 3] = (float)nearestNode->id / 100.f;
        return color;
    }
    return color;

}