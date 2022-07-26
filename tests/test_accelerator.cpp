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
