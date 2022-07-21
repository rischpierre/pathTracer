#include "raytracer.h"

/*
 * Check if a ray hits a given triangle. This is the implementation of the Möller-Trumbore algorithm
 *
 * ray: The ray that intersects or not the face.
 * face: The face that is intersected or not by the ray.
 * distance: The distance between the origin of the ray and the point of
 * intersection between the ray and the face.
 * return: true if the ray intersect the face.
 */
bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance) {

    Eigen::Vector3f edge1, edge2, p, q, t;
    float det, u, v, invertedDet;

    // find the two edges around V0
    edge1 = face->v1 - face->v0;
    edge2 = face->v2 - face->v0;

    //  get p
    p = ray->d.cross(edge2);

    det = edge1.dot(p);

    if (det > -RAY_TRACING_THRESHOLD && det < RAY_TRACING_THRESHOLD)
        return false;

    // precompute the inverse of the determinant
    invertedDet = 1.f / det;

    // distance from V to ray origin
    t = ray->o - face->v0;

    // calculate u param
    u = t.dot(p) * invertedDet;

    if (u < 0 || u > 1.f)  // the hit point is outside the triangle
        return false;

    // calculate v param
    q = t.cross(edge1);

    v = ray->d.dot(q) * invertedDet;
    if (v < 0 || u + v > 1.f)  // hit point outside triangle
        return false;

    *distance = edge2.dot(q) * invertedDet;

    // if the distance is negative, the ray is on the other side
    if (*distance > RAY_TRACING_THRESHOLD)
        return true;
    else
        return false;
}
