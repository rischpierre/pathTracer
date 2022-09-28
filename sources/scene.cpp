#include "scene.h"

void Scene::print() const{
    std::cout << "Scene:" << std::endl;
    std::cout << "  Meshes:" << std::endl;

    for (const auto& mesh: meshes){
        Shader *currentShader = nullptr;
        for (Shader shader: shaders){
            if (shader.id == mesh.shaderId){
                currentShader = &shader;
                break;
            }
        }
        std::cout << "    " << mesh.name << "(" << mesh.id << ")" << "->" << currentShader->name;
        std::cout<< "(" << currentShader->id << ")" << std::endl;
    }
};

std::string BBox::getStrRepr() const {
    Eigen::Vector3f members[2] = {min, max};

    std::string result = "(";
    int j = 0;
    for (const auto& member: members){

        for (int i = 0; i < 3; i++){
            std::string value = std::to_string(member[i]);
            result += value.substr(0, value.length() - 4);
            if (i < 2)
            result += ", ";
        }
        if (j == 0)
            result += " | ";
        j++;
    }

    result += ")";
    return result;

}

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

        pxr::GfVec3f position(0);
        position = newLight.toWorld.Transform(position);
        newLight.position = Eigen::Vector3f(position[0], position[1], position[2]);
        
        pxr::GfVec3f normal(0, 0, 1);  // lights are pointing down the Z axis by default
        normal = newLight.toWorld.Transform(normal);
        newLight.normal = Eigen::Vector3f(normal[0], normal[1], normal[2]);

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
Shader Scene::createShader(const pxr::UsdGeomMesh& mesh){

    auto bindingApi = pxr::UsdShadeMaterialBindingAPI(mesh);
    auto relationship = bindingApi.GetDirectBindingRel();
    auto direct = pxr::UsdShadeMaterialBindingAPI::DirectBinding {relationship};
    
    if (!direct.GetMaterial())
        return defaultShader;

    auto material = direct.GetMaterialPath();
    pxr::UsdPrim materialPrim = bindingApi.GetPrim().GetStage()->GetPrimAtPath(material);
    if (! materialPrim.IsValid())
        return defaultShader;

    for (const auto& shader: shaders){
        if (materialPrim.GetName().GetString() == shader.name)
            return shader;
    }

    auto previewScope = materialPrim.GetChild(pxr::TfToken("preview"));
    auto descendants = previewScope.GetChildren();

    for(auto descendant: descendants){
        if (descendant.GetTypeName() != "Shader")
            continue;

        pxr::GfVec3f diffuseColor;
        pxr::UsdAttribute diffuseAttr = descendant.GetAttribute(pxr::TfToken("inputs:diffuseColor"));

        diffuseAttr.Get(&diffuseColor, STATIC_FRAME);

        Eigen::Vector3f diffuse(diffuseColor[0], diffuseColor[1], diffuseColor[2]);

        Shader shader{diffuse, materialPrim.GetName(), shaders.back().id + 1};

        shaders.push_back(shader);
        return shader;
    }
    return defaultShader;
}

void Scene::convertUSDMeshes(const std::vector<pxr::UsdPrim> &usdMeshes){
    // populate meshes
    int faceId = 0;
    uint meshId = 0;
    for (auto &primMesh: usdMeshes) {
        pxr::UsdGeomMesh usdMesh(primMesh);
        pxr::UsdAttribute pointsAttr = usdMesh.GetPointsAttr();
        pxr::UsdAttribute normalsAttr = usdMesh.GetNormalsAttr();
        pxr::UsdAttribute fVertexIdAttr = usdMesh.GetFaceVertexIndicesAttr();
        pxr::UsdAttribute fVertexCountsAttr = usdMesh.GetFaceVertexCountsAttr();

        auto localToWorldMat = usdMesh.ComputeLocalToWorldTransform(STATIC_FRAME);

        pxr::GfBBox3d bbox = usdMesh.ComputeWorldBound(STATIC_FRAME, pxr::TfToken("default"));
        pxr::GfBBox3d bboxAA = bbox.ComputeAlignedBox();
        pxr::GfVec3d bboxMin = bboxAA.GetRange().GetMin();
        pxr::GfVec3d bboxMax = bboxAA.GetRange().GetMax();

        Eigen::Vector3f bboxMinE = Eigen::Vector3f((float)bboxMin[0], (float)bboxMin[1], (float)bboxMin[2]);
        Eigen::Vector3f bboxMaxE = Eigen::Vector3f((float)bboxMax[0], (float)bboxMax[1], (float)bboxMax[2]);

        BBox BBoxE(bboxMinE, bboxMaxE);

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
                std::cout << "Failure: face with more than 3 vertices (not implemented yet)" << std::endl;
                exit(1);
            }
        }
        Shader shader = createShader(usdMesh);

        std::vector<Eigen::Vector3f> vertices;
        std::vector<Face> faces;
        for (int vId=0; vId < fVertexIds.size(); vId = vId + 3){

            int v0Id = fVertexIds[vId];
            int v1Id = fVertexIds[vId + 1];
            int v2Id = fVertexIds[vId + 2];

            auto v0 = pxr::GfVec3f(points[v0Id][0], points[v0Id][1], points[v0Id][2]);
            auto v1 = pxr::GfVec3f(points[v1Id][0], points[v1Id][1], points[v1Id][2]);
            auto v2 = pxr::GfVec3f(points[v2Id][0], points[v2Id][1], points[v2Id][2]);

            auto n0 = pxr::GfVec3f(normals[vId][0], normals[vId][1], normals[vId][2]);
            auto n1 = pxr::GfVec3f(normals[vId + 1][0], normals[vId + 1][1], normals[vId + 1][2]);
            auto n2 = pxr::GfVec3f(normals[vId + 2][0], normals[vId + 2][1], normals[vId + 2][2]);

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

            Face face(v0e, v1e, v2e, nf, n0e, n1e, n2e, faceId, meshId, shader.id);

            faces.push_back(face);
            faceId++;
        }
        
        Mesh mesh(faces, primMesh.GetName().GetString(), BBoxE, meshId);
        meshId++;
        mesh.shaderId = shader.id;
        this->meshes.push_back(mesh);
    }

}


void Scene::parsePrimsByType(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims,
                        const pxr::TfToken& type) {
    for (pxr::UsdPrim childPrim: prim.GetChildren()) {
        if (childPrim.GetTypeName() == type) {
            rPrims.push_back(childPrim);
        }
        parsePrimsByType(childPrim, stage, rPrims, type);
    }
}

std::vector<Eigen::Vector3f> RectLight::computeSamples() const {

    int steps = LIGHT_SAMPLES;

    auto samples = std::vector<Eigen::Vector3f>(steps * steps);

    // todo refacto, this is not precise enough
    int i = 0;
    for (int x = -steps/2; x < steps/2; x++){
        for (int y = -steps/2; y < steps/2; y++) {
            auto sample = pxr::GfVec3f((float)x/this->width, (float)y/this->height, 0);
            sample = this->toWorld.Transform(sample);

            float jitter = (((float)rand() / (float)RAND_MAX) - 0.5f) * 2;

            Eigen::Vector3f sample_(sample[0] + jitter * this->width/(float)steps, sample[1] + jitter * this->height/(float)steps, sample[2]);
            samples[i] = sample_;
            i++;
        }
    }
    return samples;
}

Shader Scene::getShaderFromFace(const Face& face) const {
    for (auto shader: this->shaders) {
        if (shader.id == face.shaderId)
            return shader;
    }
    return Shader();

}
