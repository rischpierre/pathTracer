#include "directLightIntegrator.h"


Eigen::Vector3f DirectLightIntegrator::getColor(const Ray &ray, const Scene &scene) {
    float distance = WORLD_MAX_DISTANCE;
    float minDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;

    auto color = Eigen::Vector3f(0.f, 0.f, 0.f);

    for (Mesh &mesh: this->scene.meshes) {
        for (auto &face: mesh.faces) {
            bool intersected = isRayIntersectsTriangle(&ray, &face, &distance);
            if (!intersected) {
                continue;
            }

            if (distance < minDistance) {
                minDistance = distance;
                nearestFace = &face;
            }
        }
    }

    if (nearestFace) {
        Eigen::Vector3f intersectionPoint = ray.o + ray.d * minDistance;
        for (auto& light: scene.rectLights){
            Eigen::Vector3f lightDir = light.position - intersectionPoint;
            // light intensity is exposure
            color = (light.color * light.intensity * light.intensity * std::max(0.f, lightDir.dot(nearestFace->nf))) / (4 * 3.14 * lightDir.squaredNorm());
            // todo trace shadow rays
            // todo add rect light feature, now it is a point light

        }
    }
    return color;
}