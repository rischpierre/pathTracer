#include "normalIntegrator.h"


Eigen::Vector3f NormalIntegrator::getColor(const Ray &ray, const Scene& scene, bool debug) {
    float distance = WORLD_MAX_DISTANCE;
    float maxDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;
    float nearestFaceU = 0;
    float nearestFaceV = 0;

    auto color = Eigen::Vector3f(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);

    for (Mesh &mesh: this->scene.meshes){
        for (auto  &face: mesh.faces) {
            float u, v;
            bool intersected = isRayIntersectsTriangle(&ray, &face, &distance, u, v);
            if (!intersected) {
                continue;
            }

            if (distance < maxDistance) {
                maxDistance = distance;
                nearestFace = &face;
                nearestFaceU = u;
                nearestFaceV = v;
            }
        }
    }

    if (nearestFace) {
        Eigen::Vector2f uv = Eigen::Vector2f(nearestFaceU, nearestFaceV);
        // todo put that in the base class
        Eigen::Vector3f smoothNormal = (1 - uv[0] - uv[1]) * nearestFace->n0 + uv[0] * nearestFace->n1 + uv[1] * nearestFace->n2;
        smoothNormal = smoothNormal.normalized();
        for (int i = 0; i < 3; i++) {
            color[i] = (smoothNormal[i] + 1) / 2;

        }
    }
    return color;
}