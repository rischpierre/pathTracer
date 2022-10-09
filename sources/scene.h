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

struct Shader {
    Eigen::Vector3f diffuse{DEFAULT_ALBEDO, DEFAULT_ALBEDO, DEFAULT_ALBEDO};
    std::string name = "default";
    uint id = 0;
};

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
    int id;
    int shaderId;
    int meshId;

    Eigen::Vector3f getCenter() const { return (v0 + v1 + v2) / 3.0f; }
};

struct BBox {
    BBox() {}

    BBox(Eigen::Vector3f &min, Eigen::Vector3f &max) : min(min), max(max) {}

    Eigen::Vector3f min;
    Eigen::Vector3f max;

    Eigen::Vector3f center() const { return (min + max) / 2; }

    std::string getStrRepr() const;

    void print() const { std::cout << getStrRepr() << std::endl; }

    bool isFaceCenterInside(const Face &face) const {
        Eigen::Vector3f center = face.getCenter();
        for (int i = 0; i < 3; i++) {
            if (center[i] < min[i] || center[i] > max[i])
                return false;
        }

        return true;
    }
    
    bool isFaceInside(const Face &face) const {
        Eigen::Vector3f v0 = face.v0;
        Eigen::Vector3f v1 = face.v1;
        Eigen::Vector3f v2 = face.v2;

        if (v0.x() >= min.x() && v0.x() <= max.x() && v0.y() >= min.y() && v0.y() <= max.y() && v0.z() >= min.z() &&
            v0.z() <= max.z()) {
            return true;
        }
        if (v1.x() >= min.x() && v1.x() <= max.x() && v1.y() >= min.y() && v1.y() <= max.y() && v1.z() >= min.z() &&
            v1.z() <= max.z()) {
            return true;
        }

        if (v2.x() >= min.x() && v2.x() <= max.x() && v2.y() >= min.y() && v2.y() <= max.y() && v2.z() >= min.z() &&
            v2.z() <= max.z()) {
            return true;
        }
        return false;
    }
};

// Represents a mesh object in the scene. It contains faces, bbox and a shader
struct Mesh {
    Mesh(std::vector<Face> &faces, const std::string &name, const BBox &bbox, uint id)
        : faces(faces), name(name), bbox(bbox), id(id) {}

    std::vector<Face> getFaces() { return this->faces; }
    uint id;
    std::vector<Face> faces;
    std::string name;
    uint shaderId;
    BBox bbox;
};

// Represents a rectangle light 
// It contains information about its color, intensity, position and size
struct RectLight {
    float height;
    float width;

    Eigen::Vector3f color;
    float intensity;

    Eigen::Vector3f position;
    pxr::GfMatrix4d toWorld;
    Eigen::Vector3f normal;

    std::vector<Face> faces;

    void computeFaces(int startFaceId);
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
