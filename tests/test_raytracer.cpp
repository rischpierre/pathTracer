#include "gtest/gtest.h"
#include "../sources/raytracer.h"


TEST(raytracer, test_ray_inside_triangle) {
    Eigen::Vector3f o(0, 0, 0);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);

    Face f(Eigen::Vector3f(1, 2, -2),
           Eigen::Vector3f(1, 2, 1),
           Eigen::Vector3f(-2, 2, 1));

    float distance, u, v;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}

TEST(raytracer, test_ray_outside_triangle) {
    Eigen::Vector3f o(0, 0, 0);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);

    Face f(Eigen::Vector3f(-1, 2, -2),
           Eigen::Vector3f(-1, 2, 1),
           Eigen::Vector3f(-4, 2, 1));

    float distance, u, v;
    ASSERT_FALSE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}


TEST(raytracer, test_ray_inside_tilted_triangle) {
    Eigen::Vector3f o(0, 0, 0);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Face f(Eigen::Vector3f(1, 1.412, -1.34),
           Eigen::Vector3f(1, 2.95, 1.22),
           Eigen::Vector3f(-2, 2.95, 1.22));

    float distance, u, v;
    ASSERT_TRUE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}


TEST(raytracer, test_ray_outside_tilted_triangle) {
    Eigen::Vector3f o(0, 0, -1.2f);
    Eigen::Vector3f d(0, 1, 0);
    Ray ray(o, d);


    Face f(Eigen::Vector3f(1, 1.412, -1.34),
           Eigen::Vector3f(1, 2.95, 1.22),
           Eigen::Vector3f(-2, 2.95, 1.22));

    float distance, u, v;
    ASSERT_FALSE(isRayIntersectsTriangle(&ray, &f, &distance, u, v));
}
