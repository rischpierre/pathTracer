#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H


#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/attribute.h>
#include <Eigen>

#include "raytracer.h"

using namespace std;
using namespace Eigen;

class Mesh {
public:
    Mesh(vector<Face> &faces) {this->faces = faces;}

    vector<Face> getFaces() {return this->faces;}

private:
    vector<Face> faces;
};

class Camera {

public:
    explicit Camera() {};
};

class Scene {
public:
    Scene(const std::string &path);

    vector<Mesh> getMeshes() {return this->meshes;}

private:
    vector<Mesh> meshes;
};

#endif //PATHTRACER_SCENEPARSER_H
