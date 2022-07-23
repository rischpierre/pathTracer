#include "accelerator.h"


std::string Node::getStrRepr() const{
    return std::string(depth, '.') + "n" + std::to_string(id) + ": " + bbox.getStrRepr() + "\n";
}

std::string Accelerator::getStrRepr() const{
    std::string *result = new std::string;
    getNodeStrRepr(root, 0, result);
    return *result;
}

// todo overload operator std::string
void Accelerator::getNodeStrRepr(const Node& startNode, int depth, std::string* result) const {

    *result += startNode.getStrRepr();
    for(const Node* n: startNode.children){
        if (n != nullptr){
            getNodeStrRepr(*n, depth + 1, result);
        }
    }
}

void Accelerator::build(const std::vector<Mesh> &meshes){
    createMainBBbox(meshes);

    // todo try to start split in 4
    root.bbox = mainBbox;
    root.id = 0;
    root.depth = 0;
    BBox *newBBoxes = splitBBoxIn4(mainBbox);
    int boxCounter = 1;
    for (int i = 0; i < 4; i++ ){
        Node *child = new Node();
        child->bbox = newBBoxes[i];
        child->depth = 1;
        child->id = boxCounter;
        root.children[i] = child;
        boxCounter++;
    }
}


BBox* Accelerator::splitBBoxIn4(const BBox& bbox){

    static BBox bboxes[4];
    // todo get the 2 largest edges and split in 2

    float minSize = std::numeric_limits<float>::max();
    int minAxis = -1;
    for (int axis=0; axis < 3; axis++){
        float size = bbox.max[axis] - bbox.min[axis];
        if (size < minSize){
            minSize = size;
            minAxis = axis;
        }
    }

    // todo split along z, use the minAxis for that later

    Eigen::Vector3f translateY(0, (bbox.max[1] - bbox.min[1]) / 2, 0);
    Eigen::Vector3f translateX((bbox.max[0] - bbox.min[0]) / 2, 0, 0);

    int boxId = 0;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 2; j++) {
            Eigen::Vector3f childMax = bbox.max - translateX * (i ^ 1) - translateY * (j ^ 1);
            Eigen::Vector3f childMin = bbox.min + translateX * i + translateY * j;

            bboxes[boxId] = BBox(childMin, childMax);
            boxId++;
        }
    }

    return bboxes;
}
void Accelerator::createMainBBbox(const std::vector<Mesh> &meshes){

    std::vector<Mesh> meshesBbox;
    float currentMaxX = -std::numeric_limits<float>::max();
    float currentMinX = std::numeric_limits<float>::max();

    float currentMaxY = -std::numeric_limits<float>::max();
    float currentMinY = std::numeric_limits<float>::max();

    float currentMaxZ = -std::numeric_limits<float>::max();
    float currentMinZ = std::numeric_limits<float>::max();

    for (const auto& mesh: meshes){
        float minx = mesh.bbox.min.x();
        float maxx = mesh.bbox.max.x();
        float miny = mesh.bbox.min.y();
        float maxy = mesh.bbox.max.y();
        float minz = mesh.bbox.min.z();
        float maxz = mesh.bbox.max.z();

        if (minx > maxx)
            std::swap(minx, maxx);
        if (miny > maxy)
            std::swap(miny, maxy);
        if (minz > maxz)
            std::swap(minz, maxz);

        if (minx < currentMinX)
            currentMinX = minx;
        if (maxx > currentMaxX)
            currentMaxX = maxx;

        if (miny < currentMinY)
            currentMinY = miny;
        if (maxy > currentMaxY)
            currentMaxY = maxy;

        if (minz < currentMinZ)
            currentMinZ = minz;
        if (maxz > currentMaxZ)
            currentMaxZ = maxz;
    }
    Eigen::Vector3f minE(currentMinX, currentMinY, currentMinZ);
    Eigen::Vector3f maxE(currentMaxX, currentMaxY, currentMaxZ);
    this->mainBbox = BBox(minE, maxE);
}

