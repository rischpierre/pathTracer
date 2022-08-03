#include "globalIlumIntegrator.h"



void GlobalIlumIntegrator::computeShadingPoint(const Eigen::Vector2f &uv, const Face& face, const Eigen::Vector3f& hitPoint, Eigen::Vector3f& rSmoothNormal){
    // Gouraud's smooth shading technique
    // interpolate the normal of the face at the hit point
    rSmoothNormal = (1 - uv[0] - uv[1]) * face.n0 + uv[0] * face.n1 + uv[1] * face.n2;
    // we can continue here computing the hit texture coordinates

}

Eigen::Vector3f GlobalIlumIntegrator::getColor(const Ray &ray, const Scene &scene) {
    float distance = WORLD_MAX_DISTANCE;
    float minDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;
    float nearestFaceU = 0, nearestFaceV = 0;

    auto color = Eigen::Vector3f(0.f, 0.f, 0.f);

    for(Face& face: accelerator.getIntersectedFaces(ray)) {

        float u, v;
        bool intersected = isRayIntersectsTriangle(&ray, &face, &distance, u, v);
        if (!intersected) {
            continue;
        }

        if (distance < minDistance) {
            minDistance = distance;
            nearestFace = &face;
            nearestFaceU = u;
            nearestFaceV = v;

        }
    }

    if (!nearestFace)
        return color;

    Eigen::Vector3f smoothNormal;
    Eigen::Vector3f intersectionPoint = ray.o + ray.d * minDistance;
    Eigen::Vector2f uv = Eigen::Vector2f(nearestFaceU, nearestFaceV);
    computeShadingPoint(uv, *nearestFace, intersectionPoint, smoothNormal);

    smoothNormal = smoothNormal.normalized();


    for (auto& light: scene.rectLights){

        for (auto &lightSample: light.computeSamples()) {

            Eigen::Vector3f lightDir = (lightSample - intersectionPoint).normalized();

            // to avoid shadow acne
            Eigen::Vector3f shadowRayOrigin = intersectionPoint + smoothNormal * 0.00001;
            Ray shadowRay(shadowRayOrigin, lightDir);

            bool intersected = false;

            for (const Face& face: accelerator.getIntersectedFaces(shadowRay)) {

                if (face.id == nearestFace->id) {
                    continue;
                }

                float u, v;
                intersected = isRayIntersectsTriangle(&shadowRay, &face, &distance, u, v);
                if (intersected) {
                    break;
                }
            }

            if (!intersected) {
                // light intensity is exposure so its squared
                color += ((light.color * light.intensity * light.intensity *
                          std::max(0.f, lightDir.dot(smoothNormal))) /
                         (4 * 3.14 * lightDir.squaredNorm())) / (light.sampleSteps * light.sampleSteps);
            }
        }
    }
    return color;
}