
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

/*
 * Check if a ray hits a given triangle. This is the implementation of the Möller-Trumbore algorithm
 *
 * ray: The ray that intersects or not the face.
 * face: The face that is intersected or not by the ray.
 * distance: The distance between the origin of the ray and the point of
 * intersection between the ray and the face.
 * return: true if the ray intersect the face.
 */
//bool isRayIntersectsTriangle(const Ray *ray, const Face *face, float *distance) {
//
//    float edge1[3], edge2[3], p[3], q[3], t[3];
//    float det, u, v, invertedDet;
//
//    // find the two edges around V0
//    subVec3(edge1, face->v1, face->v0);
//    subVec3(edge2, face->v2, face->v0);
//
//    //  get p
//    crossProductVec3(p, ray->direction, edge2);
//
//    det = dotProductVec3(edge1, p);
//    if (det > -RAY_TRACING_THRESHOLD && det < RAY_TRACING_THRESHOLD)
//        return false;
//
//    // precompute the inverse of the determinant
//    invertedDet = 1.f / det;
//
//    // distance from V to ray origin
//    subVec3(t, ray->origin, face->v0);
//
//    // calculate u param
//    u = dotProductVec3(t, p) * invertedDet;
//    if (u < 0 || u > 1.f)  // the hit point is outside the triangle
//        return false;
//
//    // calculate v param
//    crossProductVec3(q, t, edge1);
//    v = dotProductVec3(ray->direction, q) * invertedDet;
//    if (v < 0 || u + v > 1.f)  // hit point outside triangle
//        return false;
//
//    *distance = dotProductVec3(edge2, q) * invertedDet;
//
//    return true;
//}
