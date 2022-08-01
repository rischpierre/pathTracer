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

    explicit Accelerator(const Scene& scene) : scene(scene) {
        getAllFaces();

        // init all bvh nodes
        int maxNodeNumber = allFaces.size() * 2 - 1;
        for (int i = 0; i < maxNodeNumber ; i++){
            BVHNode node;
            node.id = i;
            allNodes.push_back(node);
        }

        // init root node
        root = &allNodes[0];

    };


    void getAllFaces(){
        for (auto &mesh : scene.meshes){
            for (auto &face : mesh.faces){
                allFaces.push_back(face);
            }
        }

    };
    BVHNode build();

//    [[nodiscard]] std::string getStrRepr() const;
//    void print() const {
//        std::cout << getStrRepr() << std::endl;
//    }

    void splitBBoxIn2(const BBox& bbox, BBox& left, BBox& right);

    void print(const BVHNode& node, int depth = 0);

    void buildRecursive(BVHNode &startNode, uint8_t depth = 0);
    BBox createBBoxFromMeshes(const std::vector<Mesh> &meshes);
    BBox createBBoxFromFaces(const std::vector<Face> &faces);

    std::vector<Face> getIntersectedFaces(const Ray& ray) const;
    void getIntersectedFacesRecursive(const BVHNode& node, const Ray& ray, std::vector<int>* intersectedFaces) const;

    std::vector<BVHNode> getIntersectedNodes(const Ray &ray) const;
    void getIntersectedNodesRecursive(const BVHNode& node, const Ray &ray, std::vector<BVHNode>* nodes) const;


private:
    std::vector<BVHNode> allNodes;
    std::vector<Face> allFaces;
    int nodeIdCounter = 0;
    const Scene& scene;
    BBox mainBbox{};
    BVHNode* root{};
    uint8_t buildDepthLimit = 1;
    uint8_t buildMinFaceCount = 50;

//    void getNodeStrRepr(const BVHNode& startNode, int depth, std::string* result) const;
};




#endif //PATHTRACER_ACCELERATOR_H
