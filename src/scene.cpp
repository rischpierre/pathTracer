#include "scene.h"


Scene::Scene(const std::string &path){

    const auto stage = pxr::UsdStage::Open(path);

    // todo traverse tree instead to get all geom mesh primitives in the scene
    pxr::UsdPrim root = stage->GetPrimAtPath(pxr::SdfPath {"/"});

    vector<pxr::UsdPrim> prims;
    parseUSDMeshes(root, *stage, prims);
    std::cout << prims.empty() << endl;

    for (auto &p: prims) {
        std::cout << p.GetPath().GetString() << endl;
    }

//    auto mesh = pxr::UsdGeomMesh(primitive);
//
//    // todo lets get the prim vars like this:
//    auto *faceVertexIndices = new pxr::VtIntArray;
//    auto faceVAttr = mesh.GetFaceVertexIndicesAttr();
//    faceVAttr.Get(faceVertexIndices);
//
//    for (auto &p: *faceVertexIndices){
//        cout << p << endl;
//    }


//    mesh.GetNormalsAttr();

//    auto normalPrimVar = xform.GetPrimvar(pxr::TfToken{"normals"});
//    normalPrimVar.ComputeFlattened()
//
//    auto pointsAttr = primitive.GetAttribute(pxr::TfToken {"pointsVt"});
//    auto faceVertexIndicesAttr = primitive.GetAttribute(pxr::TfToken {"faceVertexIndices"});
//    auto normalsAttr = primitive.GetAttribute(pxr::TfToken {"normals"});
//
//    auto *normalsVt = new pxr::VtVec3fArray;
//    normalsAttr.Get(normalsVt, 0);
//
//    auto *pointsVt = new pxr::VtVec3fArray;
//    pointsAttr.Get(pointsVt, 0);
//
//    auto *faceVertexIndices = new pxr::VtIntArray;
//    faceVertexIndicesAttr.Get(faceVertexIndices, 0);
//
//    Vector3f points2[pointsVt->size()];
//    int i = 0;
//    for(auto &p: *pointsVt){
//        points2[i] = Vector3f(p[0], p[1], p[2]);
//        i++;
//    }
//    cout << points2[0][0] << endl;


    Vector3f v1(1, 10, -2);
    Vector3f v2(1, 10, 1);
    Vector3f v3(-2, 10, 1);

    Face tri(v1, v2, v3);

    vector<Face> triangles;
    triangles.push_back(tri);

    Mesh testMesh(triangles);
    this->meshes.push_back(testMesh);
}


void Scene::parseUSDMeshes(pxr::UsdPrim &prim, const pxr::UsdStage &stage, vector<pxr::UsdPrim> &rPrims) {
    // todo
    for (pxr::UsdPrim childPrim: prim.GetChildren()) {
        if (childPrim.GetTypeName() == pxr::TfToken("Mesh")) {
            rPrims.push_back(childPrim);
            parseUSDMeshes(prim, stage, rPrims);
        }
    }
}
