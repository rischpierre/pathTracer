#include "accelerator.h"
/*
Steps
- split bbox arround the main axis
- Split 2*4 times first
- Use it in the directintegartor to check if its working
- x time split

Algo:
For each node
 Split to 4
 For each child
   Get faces inside the bbox
    # to test get the faces from the parent
    If no face available then delete node
    Else add faces to the node
     If nb of faces > 20 then recurse
     Else set node to be a leaf
*/

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
    // todo start building graph recursively (4 levels)
    // todo the end nodes are leaf nodes, make it a vector in the struct
    // todo for each leaf node, link a the available meshes, if there are no faces in the box, delete the node

    // todo get all faces in the scene

    std::vector<Face> allFaces;
    for (const Mesh& mesh: meshes){
        for (const Face& face: mesh.faces){
            allFaces.push_back(face);
        }
    }

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

    // todo remove this fake recursion
    for (Node* child: root.children){
        BBox *GnewBBoxes = splitBBoxIn4(child->bbox);
        for (int j = 0; j < 4; j++ ){
            Node *Gchild = new Node();
            Gchild->bbox = GnewBBoxes[j];
            Gchild->depth = 2;
            Gchild->id = boxCounter;
            child->children[j] = Gchild;
            boxCounter++;
        }
    }

    for (Node* child: root.children){
        for (Node* Gchild: child->children) {
            BBox *GnewBBoxes = splitBBoxIn4(Gchild->bbox);
            for (int j = 0; j < 4; j++) {
                BBox currentBBox = GnewBBoxes[j];
                // todo find if there are faces in the box

                std::vector<Face> facesInBox;
                for (const Face& face: allFaces){
                    if (currentBBox.isFaceInside(face))
                        facesInBox.push_back(face);
                }

                // skip the node's creation
                if (facesInBox.empty())
                    continue;

                Node *GGchild = new Node();
                GGchild->bbox = currentBBox;
                GGchild->depth = 3;
                GGchild->id = boxCounter;
                GGchild->faces = facesInBox;
                Gchild->children[j] = GGchild;
                boxCounter++;
            }
        }
    }

    print();
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

