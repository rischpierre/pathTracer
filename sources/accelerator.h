#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H


#include "scene.h"

struct Node{
    Node* children[4] = {nullptr, nullptr, nullptr, nullptr};
    BBox bbox;
    int id = 0;
    int depth = 0;
    std::vector<Face> faces;

    [[nodiscard]] std::string getStrRepr() const;

    void print() const{
        std::cout << getStrRepr() << std::endl;
    };
};

class Accelerator {
public:
    BBox mainBbox;
    Node root;
    void build(const std::vector<Mesh> &meshes);

    [[nodiscard]] std::string getStrRepr() const;
    void print() const {
        std::cout << getStrRepr() << std::endl;
    }


private:
    void getNodeStrRepr(const Node& startNode, int depth, std::string* result) const;
    void createMainBBbox(const std::vector<Mesh> &meshes);
    BBox* splitBBoxIn4(const BBox& bbox);
};




#endif //PATHTRACER_ACCELERATOR_H
