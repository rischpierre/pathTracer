#include "scene.h"

void Scene::print() const {
    std::cout << "Scene:" << std::endl;
    std::cout << "  Meshes:" << std::endl;

    for (const auto &mesh : meshes) {
        Shader *currentShader = nullptr;
        for (Shader shader : shaders) {
            if (shader.id == mesh.shaderId) {
                currentShader = &shader;
                break;
            }
        }
        std::cout << "    " << mesh.name << "(" << mesh.id << ")"
                  << "->" << currentShader->name;
        std::cout << "(" << currentShader->id << ")" << std::endl;
    }
};

std::string BBox::getStrRepr() const {
    Eigen::Vector3f members[2] = {min, max};

    std::string result = "(";
    int j = 0;
    for (const auto &member : members) {

        for (int i = 0; i < 3; i++) {
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

    if (usdCameras.empty()) {
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

    for (const pxr::UsdPrim &light : usdLights) {
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

        // lights are pointing down the Z axis by default
        pxr::GfVec3f normal(0, 0, 1);

        normal = newLight.toWorld.Transform(normal);
        newLight.normal = Eigen::Vector3f(normal[0], normal[1], normal[2]);
        newLight.normal.normalize();

        newLight.computeFaces(faceIdCounter);

        faces.insert(faces.end(), newLight.faces.begin(), newLight.faces.end());

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

Shader Scene::createShader(const pxr::UsdGeomMesh &mesh) {

    auto bindingApi = pxr::UsdShadeMaterialBindingAPI(mesh);
    auto relationship = bindingApi.GetDirectBindingRel();
    auto direct = pxr::UsdShadeMaterialBindingAPI::DirectBinding{relationship};

    if (!direct.GetMaterial())
        return defaultShader;

    auto material = direct.GetMaterialPath();
    pxr::UsdPrim materialPrim = bindingApi.GetPrim().GetStage()->GetPrimAtPath(material);
    if (!materialPrim.IsValid())
        return defaultShader;

    for (const auto &shader : shaders) {
        if (materialPrim.GetName().GetString() == shader.name)
            return shader;
    }

    auto previewScope = materialPrim.GetChild(pxr::TfToken("preview"));
    auto descendants = previewScope.GetChildren();

    for (auto descendant : descendants) {
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

void Scene::convertUSDMeshes(const std::vector<pxr::UsdPrim> &usdMeshes) {
    uint meshId = 0;
    for (auto &primMesh : usdMeshes) {
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

        for (auto &fCount : fVertexCounts) {
            if (fCount > 3) {
                std::cout << "Failure: face with more than 3 vertices (not implemented yet)" << std::endl;
                exit(1);
            }
        }
        Shader shader = createShader(usdMesh);

        std::vector<Eigen::Vector3f> vertices;
        std::vector<Face> meshFaces;
        for (int vId = 0; vId < fVertexIds.size(); vId = vId + 3) {

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
            
            Eigen::Vector3f n0e = toEigen(n0);
            Eigen::Vector3f n1e = toEigen(n1);
            Eigen::Vector3f n2e = toEigen(n2);

            Eigen::Vector3f v0e = toEigen(v0);
            Eigen::Vector3f v1e = toEigen(v1);
            Eigen::Vector3f v2e = toEigen(v2);

            // compute Nf (face normal)
            auto nf = (v1e - v0e).cross(v2e - v0e).normalized();

            Face face(v0e, v1e, v2e, nf, n0e, n1e, n2e, faceIdCounter++, meshId, shader.id);

            meshFaces.push_back(face);
        }
        faces.insert(faces.end(), meshFaces.begin(), meshFaces.end());
        Mesh mesh(meshFaces, primMesh.GetName().GetString(), BBoxE, meshId);
        meshId++;
        mesh.shaderId = shader.id;
        this->meshes.push_back(mesh);
    }
}

void Scene::parsePrimsByType(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims,
                             const pxr::TfToken &type) {
    for (pxr::UsdPrim childPrim : prim.GetChildren()) {
        if (childPrim.GetTypeName() == type) {
            rPrims.push_back(childPrim);
        }
        parsePrimsByType(childPrim, stage, rPrims, type);
    }
}

void RectLight::computeSamples() {

    float stepSizeHeight = this->height / LIGHT_SAMPLES;
    float stepSizeWidth = this->width / LIGHT_SAMPLES;

    for (float x = -this->width / 2 + stepSizeWidth / 2; x < this->width / 2 - stepSizeWidth / 2; x += stepSizeWidth) {
        for (float y = -this->height / 2 + stepSizeHeight / 2; y < this->height / 2 - stepSizeHeight / 2;
             y += stepSizeHeight) {

            auto sample = pxr::GfVec3f(x, y, 0);
            float jitter = (((float)rand() / (float)RAND_MAX) * 2) - 1; // range: [-1, 1]

            sample = pxr::GfVec3f(sample[0] + stepSizeWidth * jitter, sample[1] + stepSizeHeight * jitter, 0);

            sample = this->toWorld.Transform(sample);
            this->samples.push_back(toEigen(sample));
        }
    }
}

void RectLight::computeFaces(int startFaceId) {
     
    int samples = 2; 
    float stepSizeHeight = this->height / samples;
    float stepSizeWidth = this->width / samples;

    pxr::GfVec3f n{0, 0, -1};
    Eigen::Vector3f ne = toEigen(this->toWorld.TransformDir(n));

    for (float x=-this->width/2; x< this->width/2; x+=stepSizeWidth) {
        for (float y=-this->height/2; y< this->height/2; y+=stepSizeHeight) {

            pxr::GfVec3f x0{x, y, 0};
            pxr::GfVec3f x1{x+stepSizeWidth, y, 0};
            pxr::GfVec3f x2{x+stepSizeHeight, y+stepSizeHeight, 0};
            pxr::GfVec3f x3{x, y+stepSizeHeight, 0};

            Eigen::Vector3f xe0 = toEigen(this->toWorld.Transform(x0));
            Eigen::Vector3f xe1 = toEigen(this->toWorld.Transform(x1));
            Eigen::Vector3f xe2 = toEigen(this->toWorld.Transform(x2));
            Eigen::Vector3f xe3 = toEigen(this->toWorld.Transform(x3));
            Face face1 = Face(xe0, xe1, xe2, ne, ne, ne, ne, startFaceId++, LIGHT_SHADER_ID, LIGHT_MESH_ID);
            Face face2 = Face(xe0, xe2, xe3, ne, ne, ne, ne, startFaceId++, LIGHT_SHADER_ID, LIGHT_MESH_ID);
            faces.push_back(face1);
            faces.push_back(face2);
        }
    }
}

Shader Scene::getShaderFromFace(const Face &face) const {
    for (auto shader : this->shaders) {
        if (shader.id == face.shaderId)
            return shader;
    }
    return Shader();
}
