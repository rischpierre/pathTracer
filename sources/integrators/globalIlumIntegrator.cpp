#include "globalIlumIntegrator.h"


ShadingPoint GlobalIlumIntegrator::computeShadingPoint(
        float u,
        float v,
        const Face& face,
        const Eigen::Vector3f& hitPoint
        ){

    // Gouraud's smooth shading technique
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
                          (4 * M_1_PI * lightDir.squaredNorm())) / (light.sampleSteps * light.sampleSteps);
            }
        }
    }
    return color;
}

Eigen::Vector3f GlobalIlumIntegrator::createHemisphereSample(const float& r1, const float& r2) {
   float sinTheta = std::sqrt(1 - r1 * r1);
    float phi = 2 * (float)M_PI * r2;
    float x = sinTheta * std::cos(phi);
    float y = sinTheta * std::sin(phi);
    return {x, y, r1};
}

Eigen::Vector3f GlobalIlumIntegrator::castRay(const Ray &ray, const Scene &scene, uint depth) {

    auto color = Eigen::Vector3f(0.f, 0.f, 0.f);

    if (depth > 3)
        return color;

    float distance = WORLD_MAX_DISTANCE;
    float minDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;
    float nearestFaceU = 0, nearestFaceV = 0;


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
    ShadingPoint shdPoint = computeShadingPoint(nearestFaceU, nearestFaceV, *nearestFace, hitPoint);

    color += getDirectContribution(ray, scene, shdPoint);

    // this is used to rotate the sample on the hemisphere
    // Eigen::Matrix2f rotationMatrix{shdPoint.n.x(), shdPoint.n.y(), shdPoint.n.y(), -shdPoint.n.x()};

    Eigen::Vector3f indirectContribution{0, 0 , 0};
//  todo need to create a new ray for each sample arround the hemisphere
// todo read about casting rays in 3d: https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing/global-illumination-path-tracing-practical-implementation

    // for (uint i = 0; i < indirectSamples; indirectSamples++){

    //     Eigen::Vector3f sample = createHemisphereSample(rand(), rand());

    //     // todo need to rotate the sample on the N of hitPoint.

    //     // Ray newRay{&shdPoint.hitPoint, sampleDir)};

    //     // todo need to compute the pdf, cdf...

    //     indirectContribution += castRay(ray, scene, depth + 1);
    // }

    float objectAlbedo = 0.18;
    color += indirectContribution / indirectSamples * objectAlbedo;

    return color;
}