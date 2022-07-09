#ifndef PATHTRACER_SCENEPARSER_H
#define PATHTRACER_SCENEPARSER_H


#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/attribute.h>

class Scene {

public:
    explicit Scene(const std::string &path);
    pxr::VtVec3fArray* getPoints();
    ~Scene();

private:
    std::vector<pxr::UsdPrim> primitives;
    pxr::VtVec3fArray *points;
};



#endif //PATHTRACER_SCENEPARSER_H
