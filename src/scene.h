#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H

#define STATIC_FRAME 0

#include <iostream>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usd/attribute.h>
#include <Eigen>



struct Face {
    Face(Eigen::Vector3f &v0,
         Eigen::Vector3f &v1,
         Eigen::Vector3f &v2,
         Eigen::Vector3f &nf,
         Eigen::Vector3f &n0,
         Eigen::Vector3f &n1,
         Eigen::Vector3f &n2) : v0(v0),
                                v1(v1),
                                v2(v2),
                                nf(nf),
                                n0(n0),
                                n1(n1),
                                n2(n2) {}

    Eigen::Vector3f v0, v1, v2, nf, n0, n1, n2;
};

struct Mesh {
    Mesh(std::vector<Face> &faces) { this->faces = faces; }

    std::vector<Face> getFaces() { return this->faces; }

    std::vector<Face> faces;
};

struct RectLight {
    float height;
    float width;

    Eigen::Vector3f color;
    float intensity;
    pxr::GfMatrix4d toWorld;   // todo maybe replace with eigen type
};

struct Camera {
    float focalLength;
    float hAperture;
    float vAperture;
    pxr::GfMatrix4d toWorld;  // todo maybe replace with eigen type
};

class Scene {
public:
    Scene(const std::string &path);

    void parsePrimsByType(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims,
                          const pxr::TfToken& type);

    void convertUSDMeshes(const std::vector<pxr::UsdPrim> &usdMeshes);

    void parseCamera(const std::vector<pxr::UsdPrim> &cameras);

    void Scene::parseLights(const std::vector<pxr::UsdPrim> &usdLights);

    std::vector<Mesh> meshes;

    std::vector<Mesh> rectLights;

    Camera camera;
};

#endif //PATHTRACER_SCENEPARSER_H
