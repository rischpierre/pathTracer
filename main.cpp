

#include <cstdio>

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/attribute.h>

int main(int argc, char *argv[]){
    printf("test\n");
    auto stage = pxr::UsdStage::Open("../exampleUSD.usda");
    auto primitive = stage->GetPrimAtPath(pxr::SdfPath {"/Cube/Cube"});
    auto attribute = primitive.GetAttribute(pxr::TfToken {"points"});

    // todo get the points of the cube, looks like a
//    pointf3
//    float value;
//    attribute.Get(&value, 0);
//    std::cout << value << std::endl;

    return 0;
}