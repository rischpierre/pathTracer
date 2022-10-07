#include "accelerator.h"

void Accelerator::print(int depth) { print(*root, depth); }

void Accelerator::print(const BVHNode &node, int depth) {
    std::cout << std::string(depth, '.') << "node" << node.id << " fCount: " << node.facesID.size();
    std::cout << node.bbox.getStrRepr() << std::endl;

    if (node.leftChild != nullptr) {
        print(*node.leftChild, depth + 1);
    }
    if (node.rightChild != nullptr) {
        print(*node.rightChild, depth + 1);
    }
}

BVHNode Accelerator::build() {

    std::vector<int> faceIds;
    for (const Face &face : allFaces) {
        faceIds.push_back(face.id);
    }

    root->facesID = faceIds;
    root->id = 0;
    root->bbox = createBBoxFromFaces(allFaces);

    buildRecursive(*root, 1);
    return *root;
}

void Accelerator::exportBBoxesToUsd(const std::string &filePath) const {
    auto stage = pxr::UsdStage::CreateNew(filePath);
    stage->SetMetadata(pxr::TfToken("upAxis"), "Z");
    stage->SetMetadata(pxr::TfToken("metersPerUnit"), 1);

    for (const auto &node : allNodes) {
        const BBox bbox = node.bbox;

        // not an initialized node
        if (node.id == -1) {
            continue;
        }

        auto mesh = pxr::UsdGeomMesh::Define(stage, pxr::SdfPath("/bbox" + std::to_string(node.id)));

        pxr::VtArray<pxr::GfVec3f> points;
        pxr::VtArray<int> faceVertexCount;
        pxr::VtArray<int> faceVertexIndices;

        // box has 8 points
        points = {
            {bbox.min.x(), bbox.min.y(), bbox.min.z()}, {bbox.max.x(), bbox.min.y(), bbox.min.z()},
            {bbox.max.x(), bbox.max.y(), bbox.min.z()}, {bbox.min.x(), bbox.max.y(), bbox.min.z()},

            {bbox.min.x(), bbox.min.y(), bbox.max.z()}, {bbox.max.x(), bbox.min.y(), bbox.max.z()},
            {bbox.max.x(), bbox.max.y(), bbox.max.z()}, {bbox.min.x(), bbox.max.y(), bbox.max.z()},
        };

        faceVertexIndices = {
            0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 5, 4, 2, 6, 7, 3, 0, 3, 7, 4, 1, 2, 6, 5,
        };
        faceVertexCount = {4, 4, 4, 4, 4, 4};

        mesh.CreatePointsAttr(pxr::VtValue(points));
        mesh.CreateFaceVertexCountsAttr(pxr::VtValue{faceVertexCount});
        mesh.CreateFaceVertexIndicesAttr(pxr::VtValue{faceVertexIndices});
    }

    std::cout << "exportBBoxesToUsd: " << filePath << std::endl;
    stage->Save();
}

void Accelerator::buildRecursive(BVHNode &startNode, uint8_t depth) {

    if (depth > ACCELERATOR_DEPTH_LIMIT)
        return;

    if (startNode.facesID.size() <= ACCELERATOR_MIN_FACES_COUNT) {
        return;
    }

    BBox leftBbox;
    BBox rightBbox;

    splitBBoxIn2(startNode.bbox, leftBbox, rightBbox);

    std::vector<int> leftFacesIds, rightFacesIds;
    std::vector<Face> leftFaces, rightFaces;
    for (const int &faceId : startNode.facesID) {

        const Face &face = allFaces[faceId];
        if (leftBbox.isFaceCenterInside(face)) {
            leftFacesIds.push_back(faceId);
            leftFaces.push_back(face);
        } else {
            rightFacesIds.push_back(faceId);
            rightFaces.push_back(face);
        }
    }

    // todo bug with the bbox having the same fCount and continuing to split
    // 0: left, 1: right
    for (uint8_t i = 0; i < 2; i++) {
        
        // skip if no faces
        if (i == 0 && leftFacesIds.empty())
            continue;
        else if (i == 1 && rightFacesIds.empty())
            continue;

        // all nodes are initialized in advance to avoid initializing them on the
        // heap
        nodeIdCounter++;
        BVHNode *childNode = &allNodes[nodeIdCounter];
        childNode->id = nodeIdCounter;

        if (i == 0) {
            childNode->facesID = leftFacesIds;
            BBox newBBox = createBBoxFromFaces(leftFaces);
            childNode->bbox = newBBox;
            startNode.leftChild = childNode;
        } else {
            childNode->facesID = rightFacesIds;
            BBox newBBox = createBBoxFromFaces(rightFaces);
            childNode->bbox = newBBox;
            startNode.rightChild = childNode;
        }

        // stop recursion to avoid having the same bbox size
        if (childNode->facesID.size() == startNode.facesID.size()) {
            continue;
        }

        buildRecursive(*childNode, depth + 1);
    }
}

void Accelerator::splitBBoxIn2(const BBox &bbox, BBox &left, BBox &right) {

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

        } else { // z
            childMax = bbox.max - translateZ * (i ^ 1);
            childMin = bbox.min + translateZ * i;
        }

        // swap values if min is over the max
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

BBox Accelerator::createBBoxFromFaces(const std::vector<Face> &faces) {

    float currentMaxX = -std::numeric_limits<float>::max();
    float currentMinX = std::numeric_limits<float>::max();
    float currentMaxY = -std::numeric_limits<float>::max();
    float currentMinY = std::numeric_limits<float>::max();
    float currentMaxZ = -std::numeric_limits<float>::max();
    float currentMinZ = std::numeric_limits<float>::max();

    for (const auto &face : faces) {
        std::vector<Eigen::Vector3f> vertices = {face.v0, face.v1, face.v2};
        for (const Eigen::Vector3f &vertex : vertices) {
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

std::vector<Face> Accelerator::getIntersectedFaces(const Ray &ray) const {
    std::vector<Face> faces;
    for (const BVHNode &node : getIntersectedNodes(ray)) {
        for (int faceId : node.facesID) {
            faces.push_back(allFaces[faceId]);
        }
    }
    return faces;
}

void Accelerator::getIntersectedNodesRecursive(const BVHNode &node, const Ray &ray, std::vector<BVHNode> *nodes) const {

    // leaf node
    if (node.leftChild == nullptr && node.rightChild == nullptr) {
        nodes->push_back(node);
        return;
    }

    if (isRayIntersectsBox(ray, node.bbox)) {
        if (node.leftChild != nullptr)
            getIntersectedNodesRecursive(*node.leftChild, ray, nodes);
        if (node.rightChild != nullptr)
            getIntersectedNodesRecursive(*node.rightChild, ray, nodes);
    }
}
