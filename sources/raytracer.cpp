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
bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance, float &u, float &v) {

    Eigen::Vector3f edge1, edge2, p, q, t;
    float det, invertedDet;

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

bool isRayIntersectsBox(const Ray& ray, const BBox& bbox){
    float tmin, tmax, tymin, tymax, tzmax, tzmin;

    // in order to avoid div by 0
    Eigen::Vector3f invDir = Eigen::Vector3f(1.f / ray.d.x(), 1.f / ray.d.y(), 1.f / ray.d.z());

    if (invDir.x() >= 0) {
        tmin = (bbox.min.x() - ray.o.x()) * invDir.x();
        tmax = (bbox.max.x() - ray.o.x()) * invDir.x();
    } else{
        tmax = (bbox.min.x() - ray.o.x()) * invDir.x();
        tmin = (bbox.max.x() - ray.o.x()) * invDir.x();
    }

    if (invDir.y() >= 0) {
        tymin = (bbox.min.y() - ray.o.y()) * invDir.y();
        tymax = (bbox.max.y() - ray.o.y()) * invDir.y();
    } else{
        tymax = (bbox.min.y() - ray.o.y()) * invDir.y();
        tymin = (bbox.max.y() - ray.o.y()) * invDir.y();
    }

    // ray misses box in y
    if ((tmin > tymax) || (tymin > tmax))
        return false;

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    if (invDir.z() >= 0) {
        tzmin = (bbox.min.z() - ray.o.z()) * invDir.z();
        tzmax = (bbox.max.z() - ray.o.z()) * invDir.z();
    }else{
        tzmax = (bbox.min.z() - ray.o.z()) * invDir.z();
        tzmin = (bbox.max.z() - ray.o.z()) * invDir.z();
    }

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;

    if (tzmin > tmin)
        tmin = tzmin;

    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

bool isRayInstersectsLight(const Ray& ray, const RectLight& light){
    
    // compute bounds vertices in local
    Eigen::Vector3f x0{-light.width/2, -light.height/2, 0};
    Eigen::Vector3f x1{-light.width/2, light.height/2, 0};
    Eigen::Vector3f x2{light.width/2, light.height/2, 0};
    Eigen::Vector3f x3{light.width/2, -light.height/2, 0};
    Eigen::Vector3f n{0, 0, -1};
    //
    // TODO transform with usd types 
    x0 = light.toWorld.Transform(x0);
    x1 = light.toWorld.Transform(x1);
    x2 = light.toWorld.Transform(x2);
    x3 = light.toWorld.Transform(x3);
    n = light.toWorld.TransformDir(n);

    Face f1 = Face(x0, x1, x2, n, n, n);
    Face f2 = Face(x0, x2, x3, n, n, n);

    float d, u, v;
    return isRayIntersectsTriangle(&ray, &f1, &d, u, v) || isRayIntersectsTriangle(&ray, &f2, &d, u, v);
}
