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



void Accelerator::print(const BVHNode& node, int depth){
    std::cout << std::string(depth, ' ') << " node: " << "fCount: " << node.facesID.size();
    std::cout << node.bbox.getStrRepr() << std::endl;

   if (node.leftChild != nullptr){
       print(*node.leftChild, depth + 1);
   }
    if (node.rightChild != nullptr){
        print(*node.rightChild, depth + 1);
    }
}

BVHNode Accelerator::build(const std::vector<Face> &faces){

    std::vector<int> faceIds;
    for (const Face& face: faces){
        allFaces.push_back(face);
        faceIds.push_back(face.id);
    }

    root = new BVHNode();
    root->facesID = faceIds;
    root->bbox = createBBoxFromFaces(allFaces);

    buildRecursive(*root, allFaces );
    print(*root);
    return *root;
}

void Accelerator::buildRecursive(BVHNode &startNode, const std::vector<Face> &faces, int depth){
    if (depth > 2)
        return;

    int minFaceCount = 50;
    if (startNode.facesID.size() <= minFaceCount){
        return;
    }

    BBox* bboxes = splitBBoxIn2(startNode.bbox);
    BBox leftBbox = bboxes[0];
    BBox rightBbox = bboxes[1];

    std::vector<int> leftFacesIds, rightFacesIds;
    for (const int& faceId: startNode.facesID){

        const Face& face = faces[faceId];
        if (leftBbox.isFaceCenterInside(face)){
            leftFacesIds.push_back(faceId);
        } else {
            rightFacesIds.push_back(faceId);
        }
    }

    // todo refacto both of these loops
    if(!leftFacesIds.empty()){
        auto* left = new BVHNode();
        left->facesID = leftFacesIds;

        // rescale bbox to fit all faces
        std::vector<Face> leftFaces;
        for (const int& faceId: leftFacesIds){
            leftFaces.push_back(faces[faceId]);
        }
        left->bbox = createBBoxFromFaces(leftFaces);

        startNode.leftChild = left;
        buildRecursive(*left, faces, depth + 1);
    }

    if(!rightFacesIds.empty()){
        auto* right = new BVHNode();
        right->facesID = rightFacesIds;

        // rescale bbox to fit all faces
        std::vector<Face> rightFaces;
        for (const int& faceId: rightFacesIds){
            rightFaces.push_back(faces[faceId]);
        }
        right->bbox = createBBoxFromFaces(rightFaces);

        startNode.rightChild = right;
        buildRecursive(*right, faces, depth + 1);
    }
}

BBox* Accelerator::splitBBoxIn2(const BBox& bbox) {

    static BBox bboxes[2];

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

    int boxId = 0;
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

        bboxes[boxId] = BBox(childMin, childMax);
        boxId++;
    }

    return bboxes;
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
    if (minAxis == -1){
        std::cerr << "Error finding axis for splitting the bbox" << std::endl;
        exit(1);
    }

    Eigen::Vector3f translateX( (bbox.max[0] - bbox.min[0]) / 2, 0, 0);
    Eigen::Vector3f translateY(0, (bbox.max[1] - bbox.min[1]) / 2, 0);
    Eigen::Vector3f translateZ(0, 0, (bbox.max[2] - bbox.min[2]) / 2);

    int boxId = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            Eigen::Vector3f childMin, childMax;

            if (minAxis == 0){ // x
                childMax = bbox.max - translateY * (i ^ 1) - translateZ * (j ^ 1);
                childMin = bbox.min + translateY * i + translateZ * j;

            } else if (minAxis ==1){ // y
                childMax = bbox.max - translateX * (i ^ 1) - translateZ * (j ^ 1);
                childMin = bbox.min + translateX * i + translateZ * j;

            } else {  // z
                childMax = bbox.max - translateX * (i ^ 1) - translateY * (j ^ 1);
                childMin = bbox.min + translateX * i + translateY * j;
            }

            //swap values if min is over the max
            for (int k = 0; k < 3; k++){
                if (childMin[k] > childMax[k]){
                    std::swap(childMin[k], childMax[k]);
                }
            }

            bboxes[boxId] = BBox(childMin, childMax);
            boxId++;
        }
    }

    return bboxes;
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

std::vector<BBox> Accelerator::getIntersectedBboxes(const Ray &ray) const {
    std::vector<BBox> bboxes;
    getIntersectedBboxesRecursive(*root, ray, &bboxes);

    return bboxes;
}

void Accelerator::getIntersectedBboxesRecursive(const BVHNode& node, const Ray &ray, std::vector<BBox>* bboxes) const {

    // leaf node
    if (node.leftChild == nullptr && node.rightChild == nullptr){
        bboxes->push_back(node.bbox);
        return;
    }

    if (isRayIntersectsBox(ray, node.bbox)){
        if (node.leftChild != nullptr)
            getIntersectedBboxesRecursive(*node.leftChild, ray, bboxes);
        if (node.rightChild != nullptr)
            getIntersectedBboxesRecursive(*node.rightChild, ray, bboxes);
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

