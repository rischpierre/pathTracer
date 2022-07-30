#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H


#include "scene.h"
#include "raytracer.h"

struct BVHNode{
    BVHNode* leftChild = nullptr;
    BVHNode* rightChild = nullptr;
    BBox bbox;
    std::vector<int> facesID;
    int id =0;
};

class Accelerator {
public:
    BBox mainBbox;
    BVHNode* root;
    BVHNode build(const std::vector<Face> &faces);

//    [[nodiscard]] std::string getStrRepr() const;
//    void print() const {
//        std::cout << getStrRepr() << std::endl;
//    }

    BBox* splitBBoxIn2(const BBox& bbox);
    BBox* splitBBoxIn4(const BBox& bbox);

    void print(const BVHNode& node, int depth = 0);

    void buildRecursive(BVHNode &startNode, const std::vector<Face> &faces, int depth=0);
    BBox createBBoxFromMeshes(const std::vector<Mesh> &meshes);
    BBox createBBoxFromFaces(const std::vector<Face> &faces);

    std::vector<Face> getIntersectedFaces(const Ray& ray) const;
    void getIntersectedFacesRecursive(const BVHNode& node, const Ray& ray, std::vector<int>* intersectedFaces) const;

    std::vector<BVHNode> getIntersectedNodes(const Ray &ray) const;
    void getIntersectedNodesRecursive(const BVHNode& node, const Ray &ray, std::vector<BVHNode>* nodes) const;

private:
    std::vector<Face> allFaces;
    int nodeIdCounter = 0;
//    void getNodeStrRepr(const BVHNode& startNode, int depth, std::string* result) const;
};




#endif //PATHTRACER_ACCELERATOR_H
