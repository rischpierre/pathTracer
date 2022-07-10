#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H


#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/attribute.h>

#include <Eigen>

using namespace std;
using namespace Eigen;

class Mesh {
    public:
        explicit Mesh(vector<Vector3f> *vertices);
        vector<Vector3f> *vertices;

        int getFaceNb();

    private:
        int faceNb;
};

class Camera {

public:
    explicit Camera() {};
};

class Scene {

public:
    explicit Scene(const std::string &path);
    vector<Mesh> getMeshes();
    ~Scene();

private:
    Mesh testMesh;
    vector<Mesh> meshes;
};

#endif //PATHTRACER_SCENEPARSER_H
