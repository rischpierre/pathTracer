#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H

#define STATIC_FRAME 0

#include <iostream>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/base/gf/bbox3d.h>
#include <pxr/usd/usdGeom/camera.h>
#include <pxr/usd/usd/attribute.h>
#include <Eigen>



struct Face {
    explicit Face(Eigen::Vector3f v0 = Eigen::Vector3f(),
         Eigen::Vector3f v1 = Eigen::Vector3f(),
         Eigen::Vector3f v2 = Eigen::Vector3f(),
         Eigen::Vector3f nf = Eigen::Vector3f(),
         Eigen::Vector3f n0 = Eigen::Vector3f(),
         Eigen::Vector3f n1 = Eigen::Vector3f(),
         Eigen::Vector3f n2 = Eigen::Vector3f(),
         int id = 0) :          v0(std::move(v0)),
                                v1(std::move(v1)),
                                v2(std::move(v2)),
                                nf(std::move(nf)),
                                n0(std::move(n0)),
                                n1(std::move(n1)),
                                n2(std::move(n2)),
                                id(id) {}
    Face(const Face& f){
        v0 = f.v0;
        v1 = f.v1;
        v2 = f.v2;
        nf = f.nf;
        n0 = f.n0;
        n1 = f.n1;
        n2 = f.n2;
        id = f.id;
    }

    Eigen::Vector3f v0, v1, v2, nf, n0, n1, n2;
    int id;

    Eigen::Vector3f getCenter() const{
        return (v0 + v1 + v2) / 3.0f;
    }
};

struct BBox {
    BBox() {}

    BBox(Eigen::Vector3f &min,
         Eigen::Vector3f &max) : min(min), max(max) {}

    Eigen::Vector3f min;
    Eigen::Vector3f max;

    Eigen::Vector3f center() const {
        return (min + max) / 2;
    }

    std::string getStrRepr() const;

    void print() const{
        std::cout << getStrRepr() << std::endl;
    }

    bool isFaceCenterInside(const Face& face) const{
        Eigen::Vector3f center = face.getCenter();
        for (int i = 0; i < 3; i++){
            if (center[i] < min[i] || center[i] > max[i])
                return false;
        }

        return true;
    }

    bool isFaceInside(const Face& face) const{
        // todo at least one of the face's vertex should be in the box
        Eigen::Vector3f v0 = face.v0;
        Eigen::Vector3f v1 = face.v1;
        Eigen::Vector3f v2 = face.v2;


        if (v0.x() >= min.x() && v0.x() <= max.x() &&
            v0.y() >= min.y() && v0.y() <= max.y() &&
            v0.z() >= min.z() && v0.z() <= max.z()){
            return true;
        }
        if  (v1.x() >= min.x() && v1.x() <= max.x() &&
             v1.y() >= min.y() && v1.y() <= max.y() &&
             v1.z() >= min.z() && v1.z() <= max.z()){
            return true;
        }

        if (v2.x() >= min.x() && v2.x() <= max.x() &&
            v2.y() >= min.y() && v2.y() <= max.y() &&
            v2.z() >= min.z() && v2.z() <= max.z()){
            return true;
        }
        return false;
    }

};


struct Mesh {
    Mesh(std::vector<Face> &faces, const std::string& name, const BBox& bbox) : faces(faces), name(name), bbox(bbox) {}

    std::vector<Face> getFaces() { return this->faces; }

    std::vector<Face> faces;
    std::string name;
    BBox bbox;

};

struct RectLight {
    float height;
    float width;

    Eigen::Vector3f color;
    Eigen::Vector3f position;
    float intensity;
    pxr::GfMatrix4d toWorld;   // todo maybe replace with eigen type

    int sampleSteps = 6;
    std::vector<Eigen::Vector3f> computeSamples() const;

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

    void parseLights(const std::vector<pxr::UsdPrim> &usdLights);

    std::vector<Mesh> meshes;

    std::vector<RectLight> rectLights;

    Camera camera;
};

#endif //PATHTRACER_SCENEPARSER_H
