

#include <cstdio>

#include <pxr/usd/usd/stage.h>

int main(int argc, char *argv[]){
    auto stage = pxr::UsdStage::CreateInMemory();
//    SdfLayer layer = SdfLayer();
    printf("test\n");

    return 0;
}