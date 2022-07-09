
/*
 *
 * Copyright (c) 2021 Pierre Risch
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of basicRaytracer.
 *
 * Author:          Pierre Risch <rischpierre@gmail.com>
 *
 */

//#include <math.h>
//#include <time.h>
//#include <malloc.h>
//#include <stdlib.h>
//#include <pthread.h>

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
bool isRayIntersectsTriangle(const Ray *ray, const Triangle *tri, float *distance) {

    Vector3f edge1, edge2, p, q, t;
    float det, u, v, invertedDet;

    // find the two edges around V0
    edge1 = tri->v1 - tri->v0;
    edge2 = tri->v2 - tri->v0;

    //  get p
    p = ray->d.cross(edge2);

    det = edge1.dot(p);

    if (det > -RAY_TRACING_THRESHOLD && det < RAY_TRACING_THRESHOLD)
        return false;

    // precompute the inverse of the determinant
    invertedDet = 1.f / det;

    // distance from V to ray origin
    t = ray->o - tri->v0;

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

    return true;
}
