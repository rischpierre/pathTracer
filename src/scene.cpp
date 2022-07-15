#include "scene.h"


Scene::Scene(const std::string &path){

    const auto stage = pxr::UsdStage::Open(path);

    pxr::UsdPrim root = stage->GetPrimAtPath(pxr::SdfPath {"/"});

    std::vector<pxr::UsdPrim> usdMeshes;
    parseUSDMeshes(root, *stage, usdMeshes);


    // populate meshes
    for (auto &primMesh: usdMeshes) {
        pxr::UsdGeomMesh usdMesh(primMesh);
        pxr::UsdAttribute pointsAttr = usdMesh.GetPointsAttr();
        pxr::UsdAttribute fVertexIdAttr = usdMesh.GetFaceVertexIndicesAttr();
        pxr::UsdAttribute fVertexCountsAttr = usdMesh.GetFaceVertexCountsAttr();

        // todo put the time somewhere else
        auto localToWorldMat = usdMesh.ComputeLocalToWorldTransform(0);

        pxr::VtVec3fArray points;
        pxr::VtIntArray fVertexIds;
        pxr::VtIntArray fVertexCounts;

        pointsAttr.Get(&points);
        fVertexIdAttr.Get(&fVertexIds);
        fVertexCountsAttr.Get(&fVertexCounts);

        for (auto &fCount: fVertexCounts){
            if (fCount > 3){
                std::cout << "Faiure: face with more than 3 vertices (not implemented yet)" << std::endl;
                exit(1);
            }
        }

        std::vector<Eigen::Vector3f> vertices;
        std::vector<Face> faces;
        for (int vId=0; vId < fVertexIds.size(); vId = vId + 3){

            int v0Id = fVertexIds[vId];
            int v1Id = fVertexIds[vId + 1];
            int v2Id = fVertexIds[vId + 2];

            auto v0 = pxr::GfVec3f(points[v0Id][0], points[v0Id][1], points[v0Id][2]);
            auto v1 = pxr::GfVec3f(points[v1Id][0], points[v1Id][1], points[v1Id][2]);
            auto v2 = pxr::GfVec3f(points[v2Id][0], points[v2Id][1], points[v2Id][2]);
            v0 = localToWorldMat.Transform(v0);
            v1 = localToWorldMat.Transform(v1);
            v2 = localToWorldMat.Transform(v2);

            auto v0e = Eigen::Vector3f(v0[0], v0[1], v0[2]);
            auto v1e = Eigen::Vector3f(v1[0], v1[1], v1[2]);
            auto v2e = Eigen::Vector3f(v2[0], v2[1], v2[2]);

            Face face(v0e, v1e, v2e);
            faces.push_back(face);
        }

        Mesh mesh(faces);
        meshes.push_back(mesh);
    }
}


void Scene::parseUSDMeshes(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims) {
    for (pxr::UsdPrim childPrim: prim.GetChildren()) {

        if (childPrim.GetTypeName() == pxr::TfToken("Mesh")) {
            rPrims.push_back(childPrim);
        }
        parseUSDMeshes(childPrim, stage, rPrims);
    }
}
