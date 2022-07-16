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

        // todo make normal integrator
        if (nearestFace) {
            Eigen::Vector3f normal = nearestFace->n.normalized();
//            std::cout << "normal: " << normal << std::endl;
            color[0] = abs(normal.x());
            color[1] = abs(normal.y());
            color[2] = abs(normal.z());
        }
    }
    return color;
}