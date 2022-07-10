#include "scene.h"

//using namespace std;

Scene::Scene(const std::string &path){

    // todo get info out of usd scene
//    auto stage = pxr::UsdStage::Open(path);
//    auto primitive = stage->GetPrimAtPath(pxr::SdfPath {"/Cube/Cube"});
//    auto attribute = primitive.GetAttribute(pxr::TfToken {"points"});
//    Scene::points = new pxr::VtVec3fArray();
//    attribute.Get(Scene::points, 0);

    Vector3f t1(1, 2, -2);
    Vector3f t2(1, 2, 1);
    Vector3f t3(-2, 2, 1);

    vector<Vector3f> vertices[3];
    vertices->push_back(t1);
    vertices->push_back(t2);
    vertices->push_back(t3);

    Mesh mesh(vertices);
    this->testMesh = mesh;
    this->meshes.push_back(this->testMesh);
}

vector<Mesh> Scene::getMeshes(){
    return Scene::meshes;
}

Scene::~Scene() {
    delete this->testMesh;
    delete[] this->meshes;
}

int Mesh::getFaceNb(){
    return this->faceNb;
}

Mesh::Mesh(vector<Vector3f> *vertices){
    this->vertices = vertices;
    this->faceNb = vertices->size();
}