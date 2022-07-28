#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H


#include "scene.h"

struct BVHNode{
    BVHNode* leftChild;
    BVHNode* rightChild;
    BBox bbox;
    std::vector<int> facesID;
};

class Accelerator {
public:
    BBox mainBbox;
    BVHNode root;
    BVHNode build(const std::vector<Face> &faces);

//    [[nodiscard]] std::string getStrRepr() const;
//    void print() const {
//        std::cout << getStrRepr() << std::endl;
//    }

    BBox* splitBBoxIn2(const BBox& bbox);
    BBox* splitBBoxIn4(const BBox& bbox);

    void print(const BVHNode& node, int depth = 0);

    void buildRecursive(BVHNode &startNode, const std::vector<Face> &faces);
    BBox createBBoxFromMeshes(const std::vector<Mesh> &meshes);
    BBox createBBoxFromFaces(const std::vector<Face> &faces);

//private:
//    void getNodeStrRepr(const BVHNode& startNode, int depth, std::string* result) const;
};




#endif //PATHTRACER_ACCELERATOR_H
