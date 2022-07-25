#include "facingRatioIntegrator.h"

Eigen::Vector3f FacingRatioIntegrator::getColor(const Ray &ray) {

    float distance = WORLD_MAX_DISTANCE;
    float maxDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;

    auto color = Eigen::Vector3f(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);

    for (Mesh &mesh: this->scene.meshes){
        for (auto  &face: mesh.faces) {
            float u, v;
            bool intersected = isRayIntersectsTriangle(&ray, &face, &distance, u ,v);
            if (!intersected) {
                continue;
            }

            if (distance < maxDistance) {
                maxDistance = distance;
                nearestFace = &face;
            }
        }

        if (nearestFace) {
            float facingRatio = nearestFace->nf.dot(-(ray.d.normalized()));

            if (facingRatio > 1 )
                facingRatio = 1.f;

            else if (facingRatio < 0)
                facingRatio = BG_COLOR_B;

            std::cout << "facingRatio: " << facingRatio << std::endl;
            for (int i = 0; i < 3; i++) {
                color[i] = facingRatio;
            }
        }
    }
    return color;
}
