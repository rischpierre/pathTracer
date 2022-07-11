#include "scene.h"


Scene::Scene(const std::string &path){

    // todo get info out of usd scene
//    auto stage = pxr::UsdStage::Open(path);
//    auto primitive = stage->GetPrimAtPath(pxr::SdfPath {"/Cube/Cube"});
//    auto attribute = primitive.GetAttribute(pxr::TfToken {"points"});
//    Scene::points = new pxr::VtVec3fArray();
//    attribute.Get(Scene::points, 0);
    Vector3f v1(1, 10, -2);
    Vector3f v2(1, 10, 1);
    Vector3f v3(-2, 10, 1);

    Face tri(v1, v2, v3);

    vector<Face> triangles;
    triangles.push_back(tri);

    Mesh testMesh(triangles);
    this->meshes.push_back(testMesh);
}


