#include "scene.h"


Scene::Scene(const std::string &path) {

    const auto stage = pxr::UsdStage::Open(path);

    pxr::UsdPrim root = stage->GetPrimAtPath(pxr::SdfPath{"/"});

    std::vector<pxr::UsdPrim> usdCameras;
    parsePrimsByType(root, *stage, usdCameras, pxr::TfToken("Camera"));

    if (usdCameras.empty()){
        std::cerr << "No cameras found in the scene" << std::endl;
        exit(1);
    }
    parseCamera(usdCameras);

    std::vector<pxr::UsdPrim> usdMeshes;
    parsePrimsByType(root, *stage, usdMeshes, pxr::TfToken("Mesh"));
    convertUSDMeshes(usdMeshes);

    std::vector<pxr::UsdPrim> usdRectLights;
    parsePrimsByType(root, *stage, usdRectLights, pxr::TfToken("RectLight"));
    parseLights(usdRectLights);

}

void Scene::parseLights(const std::vector<pxr::UsdPrim> &usdLights) {

    for (const pxr::UsdPrim &light:  usdLights){
        pxr::UsdGeomBoundable lightXForm = pxr::UsdGeomBoundable(light);
        pxr::UsdAttribute colorAttr = light.GetAttribute(pxr::TfToken("inputs:color"));
        pxr::UsdAttribute intensityAttr = light.GetAttribute(pxr::TfToken("inputs:intensity"));
        pxr::UsdAttribute widthAttr = light.GetAttribute(pxr::TfToken("inputs:width"));
        pxr::UsdAttribute heightAttr = light.GetAttribute(pxr::TfToken("inputs:height"));

        RectLight newLight;
        newLight.toWorld = lightXForm.ComputeLocalToWorldTransform(STATIC_FRAME);

        pxr::GfVec3f color(0);
        colorAttr.Get(&color, STATIC_FRAME);
        newLight.color = Eigen::Vector3f(color[0], color[1], color[2]);

        intensityAttr.Get(&newLight.intensity, STATIC_FRAME);
        widthAttr.Get(&newLight.width, STATIC_FRAME);
        heightAttr.Get(&newLight.height, STATIC_FRAME);

        this->rectLights.push_back(newLight);

    }

}

void Scene::parseCamera(const std::vector<pxr::UsdPrim> &cameras) {

    pxr::UsdGeomCamera camXform = pxr::UsdGeomCamera(cameras[0]);
    pxr::UsdAttribute focalLengthAttr = camXform.GetFocalLengthAttr();
    pxr::UsdAttribute hApertureAttr = camXform.GetHorizontalApertureAttr();
    pxr::UsdAttribute vApertureAttr = camXform.GetVerticalApertureAttr();

    this->camera = Camera();

    this->camera.toWorld = camXform.ComputeLocalToWorldTransform(STATIC_FRAME);
    focalLengthAttr.Get(&this->camera.focalLength, STATIC_FRAME);
    hApertureAttr.Get(&this->camera.hAperture, STATIC_FRAME);
    vApertureAttr.Get(&this->camera.vAperture, STATIC_FRAME);

}


void Scene::convertUSDMeshes(const std::vector<pxr::UsdPrim> &usdMeshes){
    // populate meshes
    for (auto &primMesh: usdMeshes) {
        pxr::UsdGeomMesh usdMesh(primMesh);
        pxr::UsdAttribute pointsAttr = usdMesh.GetPointsAttr();
        pxr::UsdAttribute normalsAttr = usdMesh.GetNormalsAttr();
        pxr::UsdAttribute fVertexIdAttr = usdMesh.GetFaceVertexIndicesAttr();
        pxr::UsdAttribute fVertexCountsAttr = usdMesh.GetFaceVertexCountsAttr();

        auto localToWorldMat = usdMesh.ComputeLocalToWorldTransform(STATIC_FRAME);

        pxr::VtVec3fArray points;
        pxr::VtVec3fArray normals;
        pxr::VtIntArray fVertexIds;
        pxr::VtIntArray fVertexCounts;

        pointsAttr.Get(&points);
        normalsAttr.Get(&normals);
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

            auto n0 = pxr::GfVec3f(normals[v0Id][0], normals[v0Id][1], normals[v0Id][2]);
            auto n1 = pxr::GfVec3f(normals[v1Id][0], normals[v1Id][1], normals[v1Id][2]);
            auto n2 = pxr::GfVec3f(normals[v2Id][0], normals[v2Id][1], normals[v2Id][2]);


            v0 = localToWorldMat.Transform(v0);
            v1 = localToWorldMat.Transform(v1);
            v2 = localToWorldMat.Transform(v2);

            // Transform normals to world space (only Rotation)
            n0 = localToWorldMat.TransformDir(n0);
            n1 = localToWorldMat.TransformDir(n1);
            n2 = localToWorldMat.TransformDir(n2);

            auto n0e = Eigen::Vector3f(n0[0], n0[1], n0[2]);
            auto n1e = Eigen::Vector3f(n1[0], n1[1], n1[2]);
            auto n2e = Eigen::Vector3f(n2[0], n2[1], n2[2]);

            auto v0e = Eigen::Vector3f(v0[0], v0[1], v0[2]);
            auto v1e = Eigen::Vector3f(v1[0], v1[1], v1[2]);
            auto v2e = Eigen::Vector3f(v2[0], v2[1], v2[2]);

            // compute Nf (face normal)
            auto nf = (v1e - v0e).cross(v2e - v0e).normalized();

            Face face(v0e, v1e, v2e, nf, n0e, n1e, n2e);
            faces.push_back(face);
        }

        Mesh mesh(faces);
        this->meshes.push_back(mesh);
    }
}


void
Scene::parsePrimsByType(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims,
                        const pxr::TfToken& type) {
    for (pxr::UsdPrim childPrim: prim.GetChildren()) {

        if (childPrim.GetTypeName() == type) {
            rPrims.push_back(childPrim);
        }
        parsePrimsByType(childPrim, stage, rPrims, type);
    }
}
