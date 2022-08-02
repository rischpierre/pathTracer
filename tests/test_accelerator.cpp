#include "gtest/gtest.h"
#include "../sources/accelerator.h"


TEST(accelerator, test_successfull_build){

    Scene s("../examples/test_acceleration.usda");

    Accelerator acc(s);

    acc.build();
    acc.print();
    
    // todo check if the acceleration structure is built correctly

    acc.exportBBoxesToUsd("tmp.usda");
    ASSERT_TRUE(false);
}



// todo refactore those tests
//TEST(accelerator, test_bbox_split_in_2) {
//    Accelerator acc;
//
//    Eigen::Vector3f start(0, 0, 0);
//    Eigen::Vector3f end(5, 10, 5);
//
//    BBox bbox(start, end);
//
//    BBox* newBBoxes = acc.splitBBoxIn2(bbox);
//    ASSERT_EQ(newBBoxes[0].min, Eigen::Vector3f(0, 0, 0));
//    ASSERT_EQ(newBBoxes[0].max, Eigen::Vector3f(5, 5, 5));
//
//    ASSERT_EQ(newBBoxes[1].min, Eigen::Vector3f(0, 5, 0));
//    ASSERT_EQ(newBBoxes[1].max, Eigen::Vector3f(5, 10, 5));
//}
//
//TEST(accelerator, test_build_structure) {
//    Accelerator acc;
//
//    // todo Create multiple triangles for the test
//    Face faceOrig(Eigen::Vector3f(0, 0, 0),
//                  Eigen::Vector3f(1, 0, 0),
//                  Eigen::Vector3f(0, 1, 0));
//
//    Face faceOrigBig(Eigen::Vector3f(0, 0, 0),
//                     Eigen::Vector3f(3, 0, 0),
//                     Eigen::Vector3f(0, 3, 0));
//
//    std::vector<Eigen::Vector3f> positions = {
//            Eigen::Vector3f(10, 10, 2),
//            Eigen::Vector3f(12, 12, 1),
//            Eigen::Vector3f(2, 2, 0),
//            Eigen::Vector3f(-10, -8, 0),
//    };
//
//    std::vector<Face> faces;
//    int id = 0;
//    for (const Eigen::Vector3f& pos : positions) {
//        Face f = Face(faceOrig);
//        f.v0 += pos;
//        f.v1 += pos;
//        f.v2 += pos;
//        f.id = id;
//
//        faces.push_back(f);
//        id++;
//    }
//
//    Face faceBig = Face(faceOrigBig);
//    Eigen::Vector3f transform(2, 2, 2);
//    faceBig.v0 += transform;
//    faceBig.v1 += transform;
//    faceBig.v2 += transform;
//    faceBig.id = id;
//
//    faces.push_back(faceBig);
//
//    acc.build(faces);
//    BVHNode root = acc.build(faces);
//
//    acc.print(root);
//
//    // test only first level
//    std::vector<int> expectedFacesR = {0, 1, 2, 4};
//    std::vector<int> expectedFacesL = {3};
//    ASSERT_EQ(root.leftChild->facesID, expectedFacesL);
//    ASSERT_EQ(root.rightChild->facesID, expectedFacesR);
//
//}
//
//TEST(accelerator, test_create_main_bbox) {
//    Accelerator acc;
//
//    Eigen::Vector3f none(0);
//
//    Eigen::Vector3f start(0, 0, 0);
//    Eigen::Vector3f end(10, 10, 2);
//    BBox b1(start, end);
//
//    Eigen::Vector3f start2(-10, -10, -10);
//    Eigen::Vector3f end2(0, 0, -2);
//    BBox b2(start2, end2);
//
//    Face f1, f2;
//
//    std::vector<Face> faces1 = {f1};
//    std::vector<Face> faces2 = {f2};
//
//    Mesh m1(faces1, "test1", b1);
//    Mesh m2(faces2, "test2", b2);
//
//    std::vector<Mesh> meshes = {m1, m2};
//
//    acc.createBBoxFromMeshes(meshes);
//
//    ASSERT_EQ(acc.mainBbox.min, Eigen::Vector3f(-10, -10, -10));
//    ASSERT_EQ(acc.mainBbox.max, Eigen::Vector3f(10, 10, 2));
//}
