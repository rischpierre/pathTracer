#include "gtest/gtest.h"
#include "../sources/raytracer.h"


TEST(raytracer, test_ray_inside_triangle) {
    Eigen::Vector3f o(0, 0, 0);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Eigen::Vector3f t1(1, 2, -2);
    Eigen::Vector3f t2(1, 2, 1);
    Eigen::Vector3f t3(-2, 2, 1);
    Eigen::Vector3f nf(0);
    Eigen::Vector3f n0(0);
    Eigen::Vector3f n1(0);
    Eigen::Vector3f n2(0);

    Face f(t1, t2, t3, nf, n0, n1, n2);

    float distance, u, v;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}

TEST(raytracer, test_ray_outside_triangle) {
    Eigen::Vector3f o(0, 0, 0);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Eigen::Vector3f t1(-1, 2, -2);
    Eigen::Vector3f t2(-1, 2, 1);
    Eigen::Vector3f t3(-4, 2, 1);
    Eigen::Vector3f nf(0);
    Eigen::Vector3f n0(0);
    Eigen::Vector3f n1(0);
    Eigen::Vector3f n2(0);

    Face f(t1, t2, t3, nf, n0, n1, n2);
    float distance, u, v;
    ASSERT_FALSE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}


TEST(raytracer, test_ray_inside_tilted_triangle) {
    Eigen::Vector3f o(0, 0, 0);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Eigen::Vector3f t1(1, 1.412, -1.34);
    Eigen::Vector3f t2(1, 2.95, 1.22);
    Eigen::Vector3f t3(-2, 2.95, 1.22);
    Eigen::Vector3f nf(0);
    Eigen::Vector3f n0(0);
    Eigen::Vector3f n1(0);
    Eigen::Vector3f n2(0);

    Face f(t1, t2, t3, nf, n0, n1, n2);
    float distance, u, v;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}


TEST(raytracer, test_ray_outside_tilted_triangle) {
    Eigen::Vector3f o(0, 0, -1.2f);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Eigen::Vector3f t1(1, 1.412, -1.34);
    Eigen::Vector3f t2(1, 2.95, 1.22);
    Eigen::Vector3f t3(-2, 2.95, 1.22);
    Eigen::Vector3f nf(0);
    Eigen::Vector3f n0(0);
    Eigen::Vector3f n1(0);
    Eigen::Vector3f n2(0);

    Face f(t1, t2, t3, nf, n0, n1, n2);
    float distance, u, v;
    ASSERT_FALSE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}
