#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H


#include "scene.h"

class Accelerator {
public:
    void build(const std::vector<Mesh> &meshes);
};


struct Node{
    Node* children[4];
};


#endif //PATHTRACER_ACCELERATOR_H
