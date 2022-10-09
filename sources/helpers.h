#ifndef PATHTRACER_HELPERS_H
#define PATHTRACER_HELPERS_H

#include <Eigen>
#include <png.h>
#include <pxr/usd/usd/attribute.h>


// Linear interpolation between two values
float lerpRange(float x, float x0, float x1, float y0, float y1);

// Print various vectors
void printGfMat4f(const pxr::GfMatrix4d &input);
void printGfVec3f(const pxr::GfVec3f &input);
void printVec3(const Eigen::Vector3f &v);

// Convert a usd GfVec3f to an Eigen::Vector3f
Eigen::Vector3f toEigen(const pxr::GfVec3f &input);

// Convert a GfMatrix4d to an Eigen Matrix4f
pxr::GfVec3f toGfVec3f(const Eigen::Vector3f &input);

// Helper function to write a PNG image to disk
void writePngFile(const std::string &filename, int width, int height, const Eigen::Vector3f *pixels);

#endif // PATHTRACER_HELPERS_H
