#pragma once

#include <iostream>


typedef unsigned int uint;
typedef unsigned char uchar;


template <typename T>
struct Point {
	T x;
	T y;
};


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


class ImageData {
private:
	uint _width;
	uint _height;

public:
	Color* colors;
	
	ImageData() {}
	ImageData(uint width, uint height);
	
	~ImageData();

	constexpr uint height() const {	return _height; }
	constexpr uint width() const { return _width; }

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