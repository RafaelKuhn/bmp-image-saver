#pragma once

#include <memory>      // unique_ptr
#include "types.h" // Color, Point, ImageData, uint, uchar

void write_as_bmp(const char* path, const Color* const data, unsigned int width, unsigned int height);
void write_as_bmp(const char* file_name, const ImageData &img_data);

std::unique_ptr<ImageData> read_as_bmp(const char *file_name);