#ifndef PATHTRACER_HELPERS_H
#define PATHTRACER_HELPERS_H

#include <pxr/usd/usd/attribute.h>
#include <png.h>
#include <Eigen>

float lerpRange(float x, float x0, float x1, float y0, float y1);

void printGfMat4f(const pxr::GfMatrix4d &input);

void printGfVec3f(const pxr::GfVec3f &input);


Eigen::Vector3f toEigen(const pxr::GfVec3f &input);

pxr::GfVec3f toGfVec3f(const Eigen::Vector3f &input);

// TODO cleanup camel case
void write_png_file(const std::string& filename, int width, int height, const Eigen::Vector3f *pixels);

#endif  // PATHTRACER_HELPERS_H
