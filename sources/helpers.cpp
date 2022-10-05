#include "helpers.h"

float lerpRange(float x, float x0, float x1, float y0, float y1) {
  return ((y1 - y0) * x + x1 * y0 - x0 * y1) / (x1 - x0);
}

void printGfMat4f(const pxr::GfMatrix4d &input) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      std::cout << input[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void printGfVec3f(const pxr::GfVec3f &input) {
  for (int i = 0; i < 3; i++) {
    std::cout << input[i] << " ";
  }
  std::cout << std::endl;
}

Eigen::Vector3f toEigen(const pxr::GfVec3f &input) {
  return Eigen::Vector3f(input[0], input[1], input[2]);
}

pxr::GfVec3f toGfVec3f(const Eigen::Vector3f &input) {
  return pxr::GfVec3f(input[0], input[1], input[2]);
}

// todo cleanup camel case
void write_png_file(const std::string &filename, int width, int height,
                    const Eigen::Vector3f *pixels) {

  FILE *fp = fopen(filename.c_str(), "wb");
  if (!fp)
    exit(1);

  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png)
    exit(1);

  png_infop info = png_create_info_struct(png);
  if (!info)
    exit(1);

  if (setjmp(png_jmpbuf(png)))
    abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, info);

  auto *row_pointers = (png_bytepp)png_malloc(png, height * sizeof(png_byte *));
  for (int y = 0; y < height; y++) {
    auto *row = (png_bytep)png_malloc(png, sizeof(uint8_t) * width * 3);
    row_pointers[y] = row;
    for (int x = 0; x < width; x++) {
      *row++ = std::min(1.f, pixels[y * width + x](0)) * 255;
      *row++ = std::min(1.f, pixels[y * width + x](1)) * 255;
      *row++ = std::min(1.f, pixels[y * width + x](2)) * 255;
    }
  }

  if (!row_pointers)
    abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for (int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  png_destroy_write_struct(&png, &info);
}
