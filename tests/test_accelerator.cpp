#include "gtest/gtest.h"
#include "../sources/accelerator.h"


TEST(accelerator, test_bbox_split) {
    Accelerator acc;

    Eigen::Vector3f start(0, 0, 0);
    Eigen::Vector3f end(10, 10, 5);

    BBox bbox(start, end);

    BBox* newBBoxes = acc.splitBBoxIn4(bbox);
    ASSERT_EQ(newBBoxes[0].min, Eigen::Vector3f(0, 0, 0));
    ASSERT_EQ(newBBoxes[0].max, Eigen::Vector3f(5, 5, 5));

    ASSERT_EQ(newBBoxes[1].min, Eigen::Vector3f(0, 5, 0));
    ASSERT_EQ(newBBoxes[1].max, Eigen::Vector3f(5, 10, 5));

    ASSERT_EQ(newBBoxes[2].min, Eigen::Vector3f(5, 0, 0));
    ASSERT_EQ(newBBoxes[2].max, Eigen::Vector3f(10, 5, 5));

    ASSERT_EQ(newBBoxes[3].min, Eigen::Vector3f(5, 5, 0));
    ASSERT_EQ(newBBoxes[3].max, Eigen::Vector3f(10, 10, 5));
}

TEST(accelerator, test_create_main_bbox) {
    Accelerator acc;

    Eigen::Vector3f none(0);

    Eigen::Vector3f start(0, 0, 0);
    Eigen::Vector3f end(10, 10, 2);
    BBox b1(start, end);

    Eigen::Vector3f start2(-10, -10, -10);
    Eigen::Vector3f end2(0, 0, -2);
    BBox b2(start2, end2);

    // todo how to create default args even with references in params?
    Face f1(none, none, none, none, none, none, none);
    Face f2(none, none, none, none, none, none, none);

    std::vector<Face> faces1 = {f1};
    std::vector<Face> faces2 = {f2};

    Mesh m1(faces1, "test1", b1);
    Mesh m2(faces2, "test2", b2);

    std::vector<Mesh> meshes = {m1, m2};

    acc.createMainBBbox(meshes);

    ASSERT_EQ(acc.mainBbox.min, Eigen::Vector3f(-10, -10, -10));
    ASSERT_EQ(acc.mainBbox.max, Eigen::Vector3f(10, 10, 2));
}
