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

            for (auto &lightSample: light.computeSamples()) {

                Eigen::Vector3f lightDir = (lightSample - intersectionPoint).normalized();
                // to avoid shadow acne
                Eigen::Vector3f shadowRayOrigin = intersectionPoint + nearestFace->nf * 0.0001;
                Ray shadowRay(shadowRayOrigin, lightDir);

                bool intersected = false;
                for (Mesh &mesh: this->scene.meshes) {

                    for (auto &face: mesh.faces) {

                        // skip the face that was hit by the ray
                        if (face.id == nearestFace->id) {
                            continue;
                        }

                        intersected = isRayIntersectsTriangle(&shadowRay, &face, &distance);
                        if (intersected)
                            break;
                    }
                    if (intersected)
                        break;
                }

                if (!intersected) {
                    // light intensity is exposure so its squared
                    color += ((light.color * light.intensity * light.intensity *
                              std::max(0.f, lightDir.dot(nearestFace->nf))) /
                             (4 * 3.14 * lightDir.squaredNorm())) / (light.sampleSteps * light.sampleSteps);
                }
            }
        }
    }
    return color;
}