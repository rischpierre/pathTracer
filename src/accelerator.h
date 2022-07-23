#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H


#include "scene.h"

struct Node{
    Node* children[4] = {nullptr, nullptr, nullptr, nullptr};
    BBox bbox;
    int id = 0;
};

class Accelerator {
public:
    BBox mainBbox;
    Node root;
    void build(const std::vector<Mesh> &meshes);
//    void print(const Node& startNode, int depth);

//    friend std::ostream& operator<<(const std::ostream& os, const Node& startNode);
//
//    std::string Accelerator::getRepr(const Node& startNode, int depth);

private:
    void createMainBBbox(const std::vector<Mesh> &meshes);
    BBox* splitBBoxIn4(const BBox& bbox);
};




#endif //PATHTRACER_ACCELERATOR_H
