#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H

#include <iostream>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usd/attribute.h>
#include <Eigen>


struct Face {
    Face(Eigen::Vector3f &v0, Eigen::Vector3f &v1, Eigen::Vector3f &v2) : v0(v0), v1(v1), v2(v2) {}
    Eigen::Vector3f v0, v1, v2;
};

struct Mesh {
    Mesh(std::vector<Face> &faces) {this->faces = faces;}
    std::vector<Face> getFaces() {return this->faces;}
    std::vector<Face> faces;
};

struct Camera {
};

class Scene {
public:
    Scene(const std::string &path);

    void parseUSDMeshes(pxr::UsdPrim &prim, const pxr::UsdStage &stage, std::vector<pxr::UsdPrim> &rPrims);

    std::vector<Mesh> meshes;
};

#endif //PATHTRACER_SCENEPARSER_H
