#include "normalIntegrator.h"


Eigen::Vector3f NormalIntegrator::getColor(const Ray &ray) {
    float distance = WORLD_MAX_DISTANCE;
    float maxDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;

    auto color = Eigen::Vector3f(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);

    for (Mesh &mesh: this->scene.meshes){
        for (auto  &face: mesh.faces) {

            bool intersected = isRayIntersectsTriangle(&ray, &face, &distance);
            if (!intersected) {
                continue;
            }

            if (distance < maxDistance) {
                maxDistance = distance;
                nearestFace = &face;
            }
        }

        if (nearestFace) {
            for (int i = 0; i < 3; i++) {
                color[i] = (nearestFace->nf[i] + 1) / 2;
            }
        }
    }
    return color;
}