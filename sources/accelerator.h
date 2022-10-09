#ifndef PATHTRACER_ACCELERATOR_H
#define PATHTRACER_ACCELERATOR_H

#include <pxr/usd/usdGeom/sphere.h>
#include "raytracer.h"
#include "renderSettings.h"
#include "scene.h"


// Represents a node in the BVH tree structure.
// It contains the bounding box of the node and the faces contained in it.
struct BVHNode {
    BVHNode *leftChild = nullptr;
    BVHNode *rightChild = nullptr;
    BBox bbox;
    std::vector<int> facesID;
    int id = -1;
};


// Represents a BVH tree structure accelerator.
class Accelerator {
  public:
    explicit Accelerator(const Scene &scene);

    int getNodeNumber() const { return nodeIdCounter; };
    std::vector<Face> getFaces() const { return allFaces; };
    
    // Builds the BVH tree structure recursively.
    BVHNode build();
    void buildRecursive(BVHNode &startNode, uint8_t depth = 0);
    
    // Split a given bbox in two.
    // It splits them along the longest axis.
    void splitBBoxIn2(const BBox &bbox, BBox &left, BBox &right);

    void print(int depth = 0);
    void print(const BVHNode &node, int depth = 0);
    
    // Generate a bounding box from a given set of faces
    BBox createBBoxFromFaces(const std::vector<Face> &faces);

    // Get the faces intersected by a given ray using the BVH tree structure.
    std::vector<Face> getIntersectedFaces(const Ray &ray) const;
    void getIntersectedFacesRecursive(const BVHNode &node, const Ray &ray, std::vector<int> *intersectedFaces) const;
    
    // Get the nodes intersected by a given ray using the BVH tree structure.
    std::vector<BVHNode> getIntersectedNodes(const Ray &ray) const;
    void getIntersectedNodesRecursive(const BVHNode &node, const Ray &ray, std::vector<BVHNode> *nodes) const;
    
    // Export the bounding boxes to a USD file in order to visualize them in a 3D software
    void exportBBoxesToUsd(const std::string &filePath) const;

  private:
    std::vector<BVHNode> allNodes;
    std::vector<Face> allFaces;
    int nodeIdCounter = 0;
    const Scene &scene;
    BBox mainBbox{};
    BVHNode *root{};
};

#endif // PATHTRACER_ACCELERATOR_H
