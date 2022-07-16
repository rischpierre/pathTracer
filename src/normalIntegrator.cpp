#include "normalIntegrator.h"



//normalIntegrator::integrate(const Scene &scene) {
//    float distance = WORLD_MAX_DISTANCE;
//    float maxDistance = WORLD_MAX_DISTANCE;
//    Face *nearestFace = nullptr;
//
//    for (Mesh &mesh: scene.meshes){
//        for (auto  &face: mesh.faces) {
//
//            bool intersected = isRayIntersectsTriangle(&ray, &face, &distance);
//            if (!intersected) {
//                continue;
//            }
//
//            if (distance < maxDistance) {
//                maxDistance = distance;
//                nearestFace = &face;
//            }
//        }
//        auto color = Vector3f(BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
//        // todo make normal intgrator
//        if (nearestFace) {
//            color[0] = 1;
//            color[1] = 0;
//            color[2] = 0;
//        }
//        pixels[y * RESOLUTION_W + x] = color;
//    }
//    this->scene = scene;
//}