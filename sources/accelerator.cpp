#include "accelerator.h"

//std::string BVHNode::getStrRepr() const{
//    return std::string(depth, '.') + "n" + std::to_string(id) + ": " + bbox.getStrRepr() + "\n";
//}

//std::string Accelerator::getStrRepr() const{
//    std::string *result = new std::string;
//    getNodeStrRepr(root, 0, result);
//    return *result;
//}

//void Accelerator::getNodeStrRepr(const BVHNode& startNode, int depth, std::string* result) const {
//
//    *result += startNode.getStrRepr();
//    for(const BVHNode* n: startNode.children){
//        if (n != nullptr){
//            getNodeStrRepr(*n, depth + 1, result);
//        }
//    }
//}


void Accelerator::print(int depth){
    print(*root, depth);
}

void Accelerator::print(const BVHNode& node, int depth){
    std::cout << std::string(depth, '.') << "node" << node.id << " fCount: " << node.facesID.size();
    std::cout << node.bbox.getStrRepr() << std::endl;

   if (node.leftChild != nullptr){
       print(*node.leftChild, depth + 1);
   }
    if (node.rightChild != nullptr){
        print(*node.rightChild, depth + 1);
    }
}

BVHNode Accelerator::build(){

    std::vector<int> faceIds;
    for (const Face& face: allFaces){
        allFaces.push_back(face);
        faceIds.push_back(face.id);
    }

    root->facesID = faceIds;
    root->bbox = createBBoxFromFaces(allFaces);

    buildRecursive(*root);
    print();
    return *root;
}

void Accelerator::buildRecursive(BVHNode &startNode, uint8_t depth){

    if (depth > buildDepthLimit)
        return;

    if (startNode.facesID.size() <= buildMinFaceCount){
        return;
    }

    BBox leftBbox;
    BBox rightBbox;

    splitBBoxIn2(startNode.bbox, leftBbox, rightBbox);

    std::vector<int> leftFacesIds, rightFacesIds;
    for (const int& faceId: startNode.facesID){

        const Face& face = allFaces[faceId];
        if (leftBbox.isFaceCenterInside(face)){
            leftFacesIds.push_back(faceId);
        } else {
            rightFacesIds.push_back(faceId);
        }
    }

    // todo bug with the bbox having the same fCount and continuing to split
    // 0: left, 1: right
    for (uint8_t i = 0; i < 2; i++){

        // skip if no faces
        if (i == 0 && leftFacesIds.empty())
            continue;
        else if (i == 1 && rightFacesIds.empty())
            continue;

        // all nodes are initialized in advance to avoid initializing them on the heap
        BVHNode *childNode = &allNodes[++nodeIdCounter];
        if (i == 0)
            childNode->facesID = leftFacesIds;
        else
            childNode->facesID = rightFacesIds;

        std::vector<Face> newFaces;
        for (const int& faceId: leftFacesIds){
            newFaces.push_back(allFaces[faceId]);
        }
        childNode->bbox = createBBoxFromFaces(newFaces);

        if (i == 0)
            startNode.leftChild = childNode;
        else
            startNode.rightChild = childNode;

        buildRecursive(*childNode, depth + 1);
    }
}

void Accelerator::splitBBoxIn2(const BBox& bbox, BBox& left, BBox& right) {

    float maxSize = -std::numeric_limits<float>::max();
    int maxAxis = -1;
    for (int axis = 0; axis < 3; axis++) {
        float size = bbox.max[axis] - bbox.min[axis];
        if (size > maxSize) {
            maxSize = size;
            maxAxis = axis;
        }
    }
    if (maxAxis == -1) {
        throw std::runtime_error("Error finding axis for splitting the bbox");
    }

    Eigen::Vector3f translateX((bbox.max[0] - bbox.min[0]) / 2, 0, 0);
    Eigen::Vector3f translateY(0, (bbox.max[1] - bbox.min[1]) / 2, 0);
    Eigen::Vector3f translateZ(0, 0, (bbox.max[2] - bbox.min[2]) / 2);

    for (int i = 0; i < 2; i++) {
        Eigen::Vector3f childMin, childMax;

        if (maxAxis == 0) { // x
            childMax = bbox.max - translateX * (i ^ 1);
            childMin = bbox.min + translateX * i;

        } else if (maxAxis == 1) { // y
            childMax = bbox.max - translateY * (i ^ 1);
            childMin = bbox.min + translateY * i;

        } else {  // z
            childMax = bbox.max - translateZ * (i ^ 1);
            childMin = bbox.min + translateZ * i;
        }

        //swap values if min is over the max
        for (int k = 0; k < 3; k++) {
            if (childMin[k] > childMax[k]) {
                std::swap(childMin[k], childMax[k]);
            }
        }
        if (i == 0) {
            left.min = childMin;
            left.max = childMax;
        } else {
            right.min = childMin;
            right.max = childMax;
        }
    }
}

BBox Accelerator::createBBoxFromMeshes(const std::vector<Mesh> &meshes){

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
    return BBox(minE, maxE);
}

BBox Accelerator::createBBoxFromFaces(const std::vector<Face> &faces){

    float currentMaxX = -std::numeric_limits<float>::max();
    float currentMinX = std::numeric_limits<float>::max();
    float currentMaxY = -std::numeric_limits<float>::max();
    float currentMinY = std::numeric_limits<float>::max();
    float currentMaxZ = -std::numeric_limits<float>::max();
    float currentMinZ = std::numeric_limits<float>::max();

    for (const auto& face: faces) {
        std::vector<Eigen::Vector3f> vertices = {face.v0, face.v1, face.v2};
        for(const Eigen::Vector3f& vertex: vertices){
            if (vertex.x() < currentMinX)
                currentMinX = vertex.x();
            if (vertex.x() > currentMaxX)
                currentMaxX = vertex.x();
            if (vertex.y() < currentMinY)
                currentMinY = vertex.y();
            if (vertex.y() > currentMaxY)
                currentMaxY = vertex.y();
            if (vertex.z() < currentMinZ)
                currentMinZ = vertex.z();
            if (vertex.z() > currentMaxZ)
                currentMaxZ = vertex.z();
        }
    }
    Eigen::Vector3f minE(currentMinX, currentMinY, currentMinZ);
    Eigen::Vector3f maxE(currentMaxX, currentMaxY, currentMaxZ);
    return BBox(minE, maxE);
}

std::vector<BVHNode> Accelerator::getIntersectedNodes(const Ray &ray) const {
    std::vector<BVHNode> nodes;
    getIntersectedNodesRecursive(*root, ray, &nodes);

    return nodes;
}

void Accelerator::getIntersectedNodesRecursive(const BVHNode& node, const Ray &ray, std::vector<BVHNode>* nodes) const {

    // leaf node
    if (node.leftChild == nullptr && node.rightChild == nullptr){
        nodes->push_back(node);
        return;
    }

    if (isRayIntersectsBox(ray, node.bbox)){
        if (node.leftChild != nullptr)
            getIntersectedNodesRecursive(*node.leftChild, ray, nodes);
        if (node.rightChild != nullptr)
            getIntersectedNodesRecursive(*node.rightChild, ray, nodes);
    }

}


std::vector<Face> Accelerator::getIntersectedFaces(const Ray &ray) const {
    std::vector<int> facesIDs;
    getIntersectedFacesRecursive(*root, ray, &facesIDs);

    std::vector<Face> faces;
    for (int faceID: facesIDs){
        faces.push_back(allFaces[faceID]);
    }
    return faces;
}
void Accelerator::getIntersectedFacesRecursive(const BVHNode& node, const Ray &ray, std::vector<int>* faceIDs) const {

    if (node.facesID.empty())
        return;

    // leaf node
    if (node.leftChild == nullptr && node.rightChild == nullptr){
        for (const auto& faceID: node.facesID){
            faceIDs->push_back(faceID);
        }
        return;
    }

    if (isRayIntersectsBox(ray, node.bbox)){
        getIntersectedFacesRecursive(*node.leftChild, ray, faceIDs);
        getIntersectedFacesRecursive(*node.rightChild, ray, faceIDs);
    }

}

