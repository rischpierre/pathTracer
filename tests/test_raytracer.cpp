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

#include "gtest/gtest.h"
#include "../src/raytracer.h"

#include <Eigen>
using namespace Eigen;

TEST(raytracer, test_ray_inside_triangle) {
    Vector3f o(0, 0, 0);
    Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Vector3f t1(1, 2, -2);
    Vector3f t2(1, 2, 1);
    Vector3f t3(-2, 2, 1);
    Triangle t(t1, t2, t3);

    float d;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &t, &d));
}

TEST(raytracer, test_ray_outside_triangle) {
    Vector3f o(0, 0, 0);
    Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Vector3f t1(-1, 2, -2);
    Vector3f t2(-1, 2, 1);
    Vector3f t3(-4, 2, 1);
    Triangle t(t1, t2, t3);

    float d;
    ASSERT_FALSE(isRayIntersectsTriangle(&ray, &t, &d));
}


TEST(raytracer, test_ray_inside_tilted_triangle) {
    Vector3f o(0, 0, 0);
    Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Vector3f t1(1, 1.412, -1.34);
    Vector3f t2(1, 2.95, 1.22);
    Vector3f t3(-2, 2.95, 1.22);
    Triangle t(t1, t2, t3);

    float d;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &t, &d));
}


TEST(raytracer, test_ray_outside_tilted_triangle) {
    Vector3f o(0, 0, -1.2f);
    Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Vector3f t1(1, 1.412, -1.34);
    Vector3f t2(1, 2.95, 1.22);
    Vector3f t3(-2, 2.95, 1.22);
    Triangle t(t1, t2, t3);

    float d;
    ASSERT_FALSE(isRayIntersectsTriangle(&ray, &t, &d));
}
