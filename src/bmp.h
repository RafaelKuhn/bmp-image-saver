#pragma once

#include <iostream>  // cout
#include <fstream>   // ofstream
#include <memory>    // unique_ptr

#include "types.h"   // Color, Point, ImageData, uint, uchar

void write_as_bmp(const char* path, Color* data, uint width, uint height);
void write_as_bmp(const char* file_name, std::unique_ptr<ImageData> &img_data);

std::unique_ptr<ImageData> read_as_bmp(const char *file_name);