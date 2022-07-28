#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H


#include "scene.h"

struct BVHNode{
    BVHNode* left;
    BVHNode* right;
    BBox bbox;
    int firstFaceId;
    int lastFaceId;
};

class Accelerator {
public:
    BBox mainBbox;
    BVHNode root;
    void build(const std::vector<Face> &faces);

//    [[nodiscard]] std::string getStrRepr() const;
//    void print() const {
//        std::cout << getStrRepr() << std::endl;
//    }

    BBox* splitBBoxIn2(const BBox& bbox);
    BBox* splitBBoxIn4(const BBox& bbox);

    BBox createBBoxFromMeshes(const std::vector<Mesh> &meshes);
    BBox createBBoxFromFaces(const std::vector<Face> &faces);

//private:
//    void getNodeStrRepr(const BVHNode& startNode, int depth, std::string* result) const;
};




#endif //PATHTRACER_ACCELERATOR_H
