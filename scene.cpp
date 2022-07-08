#include "scene.h"


Scene::Scene(const std::string &path){
    auto stage = pxr::UsdStage::Open(path);

    // todo fill the primitives attributes of the class
    auto primitive = stage->GetPrimAtPath(pxr::SdfPath {"/Cube/Cube"});
    auto attribute = primitive.GetAttribute(pxr::TfToken {"points"});
    Scene::points = new pxr::VtVec3fArray();

    attribute.Get(Scene::points, 0);

     // todo cleanup the scene in the destructor
}



pxr::VtVec3fArray* Scene::getPoints() {
    return Scene::points;
}

Scene::~Scene() {
    delete Scene::points;
}
