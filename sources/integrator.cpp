
#include "integrator.h"
#include "raytracer.h"
#include "renderSettings.h"

ShadingPoint Integrator::computeShadingPoint(float u, float v, const Face &face, const Eigen::Vector3f &hitPoint,
                                             const Scene &scene) {

    // Gouraud's smooth shading technique
    Eigen::Vector3f smoothNormal = ((1 - u - v) * face.n0 + u * face.n1 + v * face.n2).normalized();

    return ShadingPoint{hitPoint, smoothNormal, face, u, v, scene.getShaderFromFace(face)};
}

Eigen::Vector3f Integrator::getDirectContribution(const Ray &ray, const Scene &scene,
                                                  const ShadingPoint &shadingPoint) {

    Eigen::Vector3f color{0.f, 0.f, 0.f};
    for (RectLight light : scene.rectLights) {


        Eigen::Vector3f lightDir = (light.position - shadingPoint.hitPoint).normalized();

        // compute samples every time in order to have new samples each time, if not It will create banding
        light.computeSamples();
        for (const Eigen::Vector3f &lightSample : light.samples) {

            Eigen::Vector3f lightDirSample = (lightSample - shadingPoint.hitPoint).normalized();

            if (lightDirSample.dot(light.normal) < 0) // skip if samples is not visible to the shading point
                continue;

            // to avoid shadow acne
            Eigen::Vector3f shadowRayOrigin = shadingPoint.hitPoint + shadingPoint.n * SHADOW_BIAS;
            Ray shadowRay(shadowRayOrigin, lightDirSample);

            bool intersected = false;

            for (const Face &face : accelerator.getIntersectedFaces(shadowRay)) {

                if (face.id == shadingPoint.face.id || face.meshId == LIGHT_MESH_ID) {
                    continue;
                }

                float u, v, distance;
                intersected = isRayIntersectsTriangle(&shadowRay, &face, &distance, u, v);

                if (intersected && distance < (lightSample - shadingPoint.hitPoint).norm()) {
                    break;
                }
            }

            if (!intersected) {
                color += (light.color * light.intensity) / (4 * M_PI * lightDirSample.norm());
            }
        }
        color /= (sizeof(light.samples) /sizeof(light.samples[0]));

        // decay for shading orientation
        color *= std::max(0.f, lightDir.dot(shadingPoint.n));

        // decay for light orientation
        color *= std::max(0.f, lightDir.dot(light.normal)) / 2;
    }
    return color;
}

Eigen::Vector3f Integrator::createHemisphereSample(const float &r1, const float &r2) {
    float sinTheta = sqrtf(1 - r1 * r1);
    float phi = 2 * (float)M_PI * r2;
    float x = sinTheta * cos(phi);
    float y = sinTheta * sin(phi);
    return {x, r1, y};
}

void createCoordinateSystemFromNormal(const Eigen::Vector3f &N, Eigen::Vector3f &r_Nt, Eigen::Vector3f &r_Nb) {

    // generate a cartesian coordinate system from the normal
    // r_Nt is on the plane perpendicular to N
    if (fabs(N.x()) > fabs(N.y()))
        r_Nt = Eigen::Vector3f(N.z(), 0, -N.x()) / sqrtf(N.x() * N.x() + N.z() * N.z());
    else
        r_Nt = Eigen::Vector3f(0, -N.z(), N.y()) / sqrtf(N.y() * N.y() + N.z() * N.z());

    r_Nb = N.cross(r_Nt);
}

Eigen::Vector3f Integrator::castRay(const Ray &ray, const Scene &scene, uint depth) {

    Eigen::Vector3f hitColor{0.f, 0.f, 0.f};

    if (depth >= INDIRECT_DEPTH)
        return hitColor;

    float distance = WORLD_MAX_DISTANCE;
    float minDistance = WORLD_MAX_DISTANCE;
    Face *nearestFace = nullptr;
    float nearestFaceU = 0, nearestFaceV = 0;

    for (Face &face : accelerator.getIntersectedFaces(ray)) {

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
        return {0, 0, 0};

    if (nearestFace->meshId == LIGHT_MESH_ID) {

        // if light is backfacing the ray
        if (nearestFace->nf.dot(ray.d) > 0) {
            return {0, 0, 0};
        }
        return {1.f, 1.f, 1.f};
    }

    Eigen::Vector3f hitPoint = ray.o + ray.d * minDistance;
    ShadingPoint shdPoint = computeShadingPoint(nearestFaceU, nearestFaceV, *nearestFace, hitPoint, scene);

    Eigen::Vector3f directContribution = getDirectContribution(ray, scene, shdPoint);

#if DEBUG_PIXEL == true
    std::string depthTab(depth * 2, ' ');
    std::cout << depthTab << "DC: " << directContribution[0] << " ";
    std::cout << directContribution[1] << " " << directContribution[2] << " mesh: ";
    std::cout << scene.meshes[shdPoint.face.meshId].name << " fId: ";
    std::cout << shdPoint.face.id << std::endl;
#endif

    Eigen::Vector3f indirectContribution{0, 0, 0};

    Eigen::Vector3f Nb{0, 0, 0};
    Eigen::Vector3f Nt{0, 0, 0};

    createCoordinateSystemFromNormal(shdPoint.n, Nt, Nb);
    float pdf = 1 / (2 * M_PI);

    for (uint i = 0; i < INDIRECT_SAMPLES; i++) {
        float r1 = (float)rand() / RAND_MAX;
        float r2 = (float)rand() / RAND_MAX;

        Eigen::Vector3f sample = createHemisphereSample(r1, r2);

        // transform sample with the coordinate system
        Eigen::Vector3f sampleWorld = {sample.x() * Nb.x() + sample.y() * shdPoint.n.x() + sample.z() * Nt.x(),
                                       sample.x() * Nb.y() + sample.y() * shdPoint.n.y() + sample.z() * Nt.y(),
                                       sample.x() * Nb.z() + sample.y() * shdPoint.n.z() + sample.z() * Nt.z()};

        // r1 is cos(theta)
        Ray newRay(hitPoint + shdPoint.n * INDIRECT_RAY_BIAS, sampleWorld);

        indirectContribution += r1 * castRay(newRay, scene, depth + 1) / pdf;
    }
    indirectContribution /= (float)INDIRECT_SAMPLES;

    hitColor = (directContribution / M_PI + indirectContribution / 2).cwiseProduct(shdPoint.shader.diffuse);

#if DEBUG_PIXEL == true
    std::cout << depthTab << "IC: " << indirectContribution[0] << " ";
    std::cout << indirectContribution[1] << " " << indirectContribution[2] << std::endl;
#endif

    return hitColor;
}
