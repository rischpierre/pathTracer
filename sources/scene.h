#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H

#include <iostream>

#include <Eigen>
#include <pxr/base/gf/bbox3d.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>

#include "helpers.h"
#include "renderSettings.h"

// Shader definition, only diffuse information for now
struct Shader {
    Eigen::Vector3f diffuse{DEFAULT_ALBEDO, DEFAULT_ALBEDO, DEFAULT_ALBEDO};
    std::string name = "default";
    uint id = 0;
};

// Face (triangle) definition containing the vertices, id and normals
struct Face {
    explicit Face(Eigen::Vector3f v0 = Eigen::Vector3f(), Eigen::Vector3f v1 = Eigen::Vector3f(),
                  Eigen::Vector3f v2 = Eigen::Vector3f(), Eigen::Vector3f nf = Eigen::Vector3f(),
                  Eigen::Vector3f n0 = Eigen::Vector3f(), Eigen::Vector3f n1 = Eigen::Vector3f(),
                  Eigen::Vector3f n2 = Eigen::Vector3f(), int id = 0, int meshId = 0, int shaderId = 0)
        : v0(std::move(v0)), v1(std::move(v1)), v2(std::move(v2)), nf(std::move(nf)), n0(std::move(n0)),
          n1(std::move(n1)), n2(std::move(n2)), id(id), meshId(meshId), shaderId(shaderId) {}
    Face(const Face &f) {
        v0 = f.v0;
        v1 = f.v1;
        v2 = f.v2;
        nf = f.nf;
        n0 = f.n0;
        n1 = f.n1;
        n2 = f.n2;
        id = f.id;
        shaderId = f.shaderId;
        meshId = f.meshId;
    }

    Eigen::Vector3f v0, v1, v2, nf, n0, n1, n2;
    int id, shaderId, meshId;

    Eigen::Vector3f getCenter() const;
};

// Represents a bounding box in the scene on the min, max coordinate model.
struct BBox {
    BBox() {}

    BBox(Eigen::Vector3f &min, Eigen::Vector3f &max) : min(min), max(max) {}

    Eigen::Vector3f min;
    Eigen::Vector3f max;

    std::string getStrRepr() const;

    void print() const { std::cout << getStrRepr() << std::endl; }

    bool isFaceCenterInside(const Face &face) const;

    bool isFaceInside(const Face &face) const;
};

// Represents a mesh object in the scene. It contains faces, bbox and a shader
struct Mesh {
    Mesh(std::vector<Face> &faces, const std::string &name, const BBox &bbox, uint id)
        : faces(faces), name(name), bbox(bbox), id(id) {}

    uint id, shaderId;
    std::vector<Face> faces;
    std::string name;
    BBox bbox;
};

// Represents a rectangle light
// It contains information about its color, intensity, position and size
struct RectLight {
    float width, height, intensity;

    Eigen::Vector3f color, position, normal;

    pxr::GfMatrix4d toWorld;

    std::vector<Face> faces;
    
    // Generates faces from the light in order to add the resulting faces in the BVH
    void computeFaces(int startFaceId);

    // Generates a set of random points on the light
    // todo maybe use a array instead of a vector to be faster?
    std::vector<Eigen::Vector3f> computeSamples();
};

struct Camera {
    float focalLength;
    float hAperture;
    float vAperture;
    pxr::GfMatrix4d toWorld;
};

// Container and manager of all objects in the scene
// It contains all meshes, lights, camera and shaders
// It also converts USD data to internal data structures
class Scene {
  public:
    Scene(const std::string &path);

    void parsePrimsByType(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims,
                          const pxr::TfToken &type);

    void convertUSDMeshes(const std::vector<pxr::UsdPrim> &usdMeshes);

    void parseCamera(const std::vector<pxr::UsdPrim> &cameras);

    void parseLights(const std::vector<pxr::UsdPrim> &usdLights);

    Shader getShaderFromFace(const Face &face) const;

    Shader createShader(const pxr::UsdGeomMesh &mesh);

    void print() const;

    std::vector<Mesh> meshes;
    std::vector<Face> faces;
    std::vector<RectLight> rectLights;
    Camera camera;

    Shader defaultShader;
    std::vector<Shader> shaders = {defaultShader};
    int faceIdCounter = 0;
};

#endif // PATHTRACER_SCENEPARSER_H
