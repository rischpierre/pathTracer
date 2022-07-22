#include "accelerator.h"


void Accelerator::build(const std::vector<Mesh> &meshes){
    
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
    BBox mainBbox(minE, maxE);

}
