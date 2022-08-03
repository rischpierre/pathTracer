#include "globalIlumIntegrator.h"



ShadingPoint GlobalIlumIntegrator::computeShadingPoint(
        float u,
        float v,
        const Face& face,
        const Eigen::Vector3f& hitPoint
        ){

    // Gouraud's smooth shading technique
    // interpolate the normal of the face at the hit point
    Eigen::Vector3f smoothNormal = ((1 - u - v) * face.n0 + u * face.n1 + v * face.n2).normalized();

    return ShadingPoint {
        hitPoint,
        smoothNormal,
        face,
        u,
        v
    };

}

Eigen::Vector3f GlobalIlumIntegrator::getDirectContribution(
        const Ray &ray,
        const Scene &scene,
        const ShadingPoint& shadingPoint) {

    Eigen::Vector3f color{0.f, 0.f, 0.f};
    for (auto& light: scene.rectLights){

        for (auto &lightSample: light.computeSamples()) {

            Eigen::Vector3f lightDir = (lightSample - shadingPoint.hitPoint).normalized();

            // to avoid shadow acne
            Eigen::Vector3f shadowRayOrigin = shadingPoint.hitPoint + shadingPoint.n * 0.00001;
            Ray shadowRay(shadowRayOrigin, lightDir);

            bool intersected = false;

            for (const Face& face: accelerator.getIntersectedFaces(shadowRay)) {

                if (face.id == shadingPoint.face.id) {
                    continue;
                }

                float u, v, distance;
                intersected = isRayIntersectsTriangle(&shadowRay, &face, &distance, u, v);
                if (intersected) {
                    break;
                }
            }

            if (!intersected) {
                // light intensity is exposure so its squared
                color += ((light.color * light.intensity * light.intensity *
                           std::max(0.f, lightDir.dot(shadingPoint.n))) /
                          (4 * 3.14 * lightDir.squaredNorm())) / (light.sampleSteps * light.sampleSteps);
            }
        }
    }
    return color;
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
    Eigen::Vector3f hitPoint = ray.o + ray.d * minDistance;
    ShadingPoint shadingPoint = computeShadingPoint(
        nearestFaceU,
        nearestFaceV,
        *nearestFace,
        hitPoint
    );

    color += getDirectContribution(ray, scene, shadingPoint);

    return color;
}