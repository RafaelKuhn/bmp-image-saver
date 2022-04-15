#pragma once

#include <memory>      // unique_ptr
#include "bmp-types.h" // Color, Point, ImageData, uint, uchar

void write_as_bmp(const char* path, const Color *data, unsigned int width, unsigned int height);
void write_as_bmp(const char* file_name, ImageData &img_data);

std::unique_ptr<ImageData> read_as_bmp(const char *file_name);