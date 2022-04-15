#pragma once

typedef unsigned int uint;
typedef unsigned char uchar;

#ifdef DEBUG_MODE
#include <iostream>
#endif

template <typename T>
struct Point {
	T x;
	T y;
};

/// Data container for rgb color
struct Color {
	char r{0}, g{0}, b{0};

	Color();
	Color(int r, int g, int b);
	Color(char *little_endian_3byte_bgr_arr);

#ifdef DEBUG_MODE
	void* operator new(size_t size);
	void* operator new[](size_t size);
	void operator delete(void * p, size_t size);
	void operator delete[](void * p, size_t size);
#endif
};

/// Data container for image width, height and pointer to pixel colors
class ImageData {
private:
	uint _width;
	uint _height;

public:
	// TODO: could be a shared pointer, but too much overhead for my nooby c++ brain
	Color* colors;
	
	// ImageData() {} // TODO: should remove this?
	ImageData(uint width, uint height);
	
	~ImageData();

	constexpr uint get_height() const {	return _height; }
	constexpr uint get_width() const { return _width; }

#ifdef DEBUG_MODE
	void* operator new(size_t size);
	void* operator new[](size_t size);
	void operator delete(void * p, size_t size);
	void operator delete[](void * p, size_t size);
#endif
};

#ifdef DEBUG_MODE
class MemoryMetric {
private:
	MemoryMetric();

public:
	static MemoryMetric& get_instance();
	uint bytes = 0;
	void print_metrics();
	void allocate_and_print(size_t size);
	void release_and_print(size_t size);
};
#endif