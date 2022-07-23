#include "accelerator.h"


// todo overload operator std::string
//std::string Accelerator::getRepr(const Node& startNode, int depth){
//
//    std::string result;
//    std::string tabs(depth, '-');
//
//    result = tabs + "n" + std::to_string(startNode.id) + " (";
//    // min
//    // todo make a print directly to bbox class
//    for (int i = 0; i < 3; i++){
//        // * 10 /10 to round u numbers
//        std::cout << round(startNode.bbox.min[i] * 10.f)/10.f;
//        if (i < 2)
//            std::cout << ", ";
//    }
//    std::cout << " | ";
//
//    // max
//    for (int i = 0; i < 3; i++){
//        std::cout << round(startNode.bbox.max[i] * 10.f)/10.f;
//        if (i < 2)
//            std::cout << ", ";
//    }
//
//    std::cout << ")" << std::endl;
//
//    for(const Node* n: startNode.children){
//        if (n != nullptr){
//            getRepr(*n, depth + 1);
//        }
//    }
//    return result;
//}

void Accelerator::build(const std::vector<Mesh> &meshes){
    createMainBBbox(meshes);

    // todo try to start split in 4
    root.bbox = mainBbox;
    root.id = 0;
    BBox *newBBoxes = splitBBoxIn4(mainBbox);
    int boxCounter = 1;
    for (int i = 0; i < 4; i++ ){
        Node *child = new Node();
        child->bbox = newBBoxes[i];
        child->id = boxCounter;
        root.children[i] = child;
        boxCounter++;
    }

    std::cout << std::string(root.bbox) << std::endl;
    exit(0);
}


//void Accelerator::createChildren(){
//
//}



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

