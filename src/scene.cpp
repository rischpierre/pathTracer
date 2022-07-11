#include "scene.h"


Scene::Scene(const std::string &path){

    // todo get info out of usd scene
    auto stage = pxr::UsdStage::Open(path);
    auto primitive = stage->GetPrimAtPath(pxr::SdfPath {"/Cube/Cube"});
    auto pointsAttr = primitive.GetAttribute(pxr::TfToken {"pointsVt"});
    auto faceVertexIndicesAttr = primitive.GetAttribute(pxr::TfToken {"faceVertexIndices"});
    auto normalsAttr = primitive.GetAttribute(pxr::TfToken {"normals"});

    auto *normalsVt = new pxr::VtVec3fArray;
    normalsAttr.Get(normalsVt, 0);

//    cout << normalsVt[0] << endl;

    auto *pointsVt = new pxr::VtVec3fArray;
    pointsAttr.Get(pointsVt, 0);

    auto *faceVertexIndices = new pxr::VtIntArray;
    faceVertexIndicesAttr.Get(faceVertexIndices, 0);

    Vector3f points2[pointsVt->size()];
    int i = 0;
    for(auto &p: *pointsVt){
        points2[i] = Vector3f(p[0], p[1], p[2]);
        i++;
    }
    cout << points2[0][0] << endl;


    Vector3f v1(1, 10, -2);
    Vector3f v2(1, 10, 1);
    Vector3f v3(-2, 10, 1);

    Face tri(v1, v2, v3);

    vector<Face> triangles;
    triangles.push_back(tri);

    Mesh testMesh(triangles);
    this->meshes.push_back(testMesh);
}


