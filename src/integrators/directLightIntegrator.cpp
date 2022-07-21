#include "directLightIntegrator.h"


Eigen::Vector3f DirectLightIntegrator::getColor(const Ray &ray, const Scene &scene) {
    float distance = WORLD_MAX_DISTANCE;
    float minDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;
    Mesh *intersectedMesh = nullptr;

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
                intersectedMesh = &mesh;
            }
        }
    }

    if (nearestFace) {
        Eigen::Vector3f intersectionPoint = ray.o + ray.d * minDistance;
        for (auto& light: scene.rectLights){
            Eigen::Vector3f lightDir = (light.position - intersectionPoint).normalized();

            Eigen::Vector3f position = light.position;

            Eigen::Vector3f shadowRayOrigin = intersectionPoint + nearestFace->nf * 0.1;

            Ray shadowRay(shadowRayOrigin, lightDir);

            bool intersected = false;
            for (Mesh &mesh: this->scene.meshes) {
                if (mesh.name == intersectedMesh->name) {
                    continue;
                }

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
                color += (light.color * light.intensity * light.intensity *
                         std::max(0.f, lightDir.dot(nearestFace->nf))) / (4 * 3.14 * lightDir.squaredNorm());
                // todo add rect light feature, now it is a point light
            }
        }
    }
    return color;
}