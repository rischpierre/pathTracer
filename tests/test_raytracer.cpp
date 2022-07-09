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
    Vector3f v0(0, 0, 0);
    Vector3f v1(0, 1, 0);
    Ray ray(v0, v1);


    Vector3f t1(0, 0, 0);
    Vector3f t2(0, 1, 0);
    Vector3f t3(1, 0, 0);
    Triangle t(t1, t2, t3);

    float d;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &t, &d));
}

//TEST(raytracer, test_ray_outside_triangle) {
//    Ray ray = {{0, 0, 0},
//               {0, 1, 0}};
//
//    float d;
//    Scene *s = (Scene *) malloc(sizeof(Scene));
//    parseObjFile(s, "../../examples/notIntersectingYTriangle.obj");
//    ASSERT_FALSE(isRayIntersectsTriangle(&ray, s->object.faces, &d));
//    freeScene(s);
//}
//
//
//TEST(raytracer, test_ray_inside_tilted_triangle) {
//    Ray ray = {{0, 0, 0},
//               {0, 1, 0}};
//
//    Scene *s = (Scene *) malloc(sizeof(Scene));
//    float d;
//    parseObjFile(s, "../../examples/tiltedTriangle.obj");
//    ASSERT_TRUE(isRayIntersectsTriangle(&ray, s->object.faces, &d));
//    freeScene(s);
//}
//
//TEST(raytracer, test_ray_outside_tilted_triangle) {
//    Ray ray = {{0, 0, -1.2f},
//               {0, 1, 0}};
//
//    Scene *s = (Scene *) malloc(sizeof(Scene));
//    float d;
//    parseObjFile(s, "../../examples/tiltedTriangle.obj");
//    ASSERT_FALSE(isRayIntersectsTriangle(&ray, s->object.faces, &d));
//    freeScene(s);
//}
//
//TEST(computeColor, test_computeColor){
//    float objectColor[3] = {1, 1, 1};
//    DirLight light = {{1, 0, 0}};
//    float faceN[3] = {-1, 0, 0};
//    float color[3] = {0, 0, 0};
//    computeColor(color, faceN, &light, objectColor);
//    ASSERT_EQ(color[0], 1.f);
//
//    faceN[0] = -0.5f;
//    faceN[1] = -0.5f;
//    faceN[2] = 0;
//    float color2[3] = {0, 0, 0};
//    computeColor(color2, faceN, &light, objectColor);
//    ASSERT_FLOAT_EQ(color2[0], 0.5f);
//
//    faceN[0] = 1.f;
//    faceN[1] = 0;
//    faceN[2] = 0;
//    float color3[3] = {0, 0, 0};
//    computeColor(color3, faceN, &light, objectColor);
//    ASSERT_FLOAT_EQ(color3[0], BG_COLOR_R);
//}
//
//TEST(split_quads, test_only_tri_faces) {
//
//    Object o;
//    Face f;
//    Face f2;
//    f.isQuad = false;
//    f2.isQuad = false;
//
//    Face faces[2] = {f2, f};
//    o.faces = faces;
//    o.faceNb = 2;
//
//    splitQuads(&o);
//    ASSERT_EQ(o.faceNb, 2);
//}
//
//TEST(split_quads_2, test_with_quad_faces) {
//
//    Object o;
//    Face f = {
//            {1, 2, 3},
//            {4, 5, 6},
//            {7, 8, 9},
//            {10, 11, 12},
//            true,
//            {13, 14, 15}
//    };
//    Face *faces = (Face *) malloc(sizeof(Face) * 1);
//    faces[0] = f;
//    o.faces = faces;
//    o.faceNb = 1;
//
//    splitQuads(&o);
//
//    ASSERT_EQ(o.faceNb, 2);
//    ASSERT_EQ(o.faces[0].v0[0], 1);
//    ASSERT_EQ(o.faces[0].v1[2], 6);
//
//    ASSERT_EQ(o.faces[1].v0[0], 1);
//    ASSERT_EQ(o.faces[1].v1[2], 9);
//
//    ASSERT_EQ(o.faces[1].n[0], 13);
//    ASSERT_EQ(o.faces[1].n[2], 15);
//
//    free(o.faces);
//}

