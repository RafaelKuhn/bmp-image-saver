#include "bmp-types.h"

#ifdef DEBUG_MODE
#include <iostream> // cout
#endif

Color::Color() { }
Color::Color(int r, int g, int b): r(r), g(g), b(b) { }
Color::Color(const char* const little_endian_3byte_bgr_arr) {
	const uint* int_ptr = reinterpret_cast<const uint *>(little_endian_3byte_bgr_arr);
	r = (char)(*int_ptr >> 8 * 2);
	g = (char)(*int_ptr >> 8 * 1);
	b = (char)(*int_ptr >> 8 * 0);
};


ImageData::ImageData(uint width, uint height): _width(width), _height(height) {
	colors = new Color[width * height];
}

ImageData::~ImageData() {
	delete[] colors;
}



#ifdef DEBUG_MODE

MemoryMetric::MemoryMetric() { }

/// Singleton to store memory heap allocation metrics
MemoryMetric& MemoryMetric::get_instance() {
	static MemoryMetric _instance;
	return _instance;
}

void MemoryMetric::print_metrics() {
	std::cout << "[bmp debug] heap memory in use: " << bytes << " B | " << bytes/1024 << " KB\n";
}

void MemoryMetric::allocate_and_print(size_t size) {
	bytes += size;
	std::cout << "[bmp debug] heap memory in use: " << bytes << " B | " << bytes/1024 << " KB, allocated " << size << " B | " << size/1024 << " KB \n";
}
void MemoryMetric::release_and_print(size_t size) {
	bytes -= size;
	std::cout << "[bmp debug] heap memory in use: " << bytes << " B | " << bytes/1024 << " KB, released " << size << " B | " << size/1024 << " KB\n";
}


// override heap allocation methods for Colors
void* Color::operator new(size_t size) {
	MemoryMetric::get_instance().allocate_and_print(size);
	void * p = ::operator new(size); // here, :: returns one scope
	return p;
}

void* Color::operator new[](size_t size) {
	MemoryMetric::get_instance().allocate_and_print(size);
	void *p = malloc(size); // same as ::operator new(size)
	return p;
}

void Color::operator delete(void *p, size_t size) {
	MemoryMetric::get_instance().release_and_print(size);
	free(p);
}
void Color::operator delete[](void *p, size_t size) {
	MemoryMetric::get_instance().release_and_print(size);
	free(p);
}

// override heap allocation methods for ImageData
void* ImageData::operator new(size_t size) {
	MemoryMetric::get_instance().allocate_and_print(size);
	void *p = malloc(size);
	return p;
}

void* ImageData::operator new[](size_t size) {
	MemoryMetric::get_instance().allocate_and_print(size);
	void *p = malloc(size);
	return p;
}

void ImageData::operator delete(void *p, size_t size) {
	MemoryMetric::get_instance().release_and_print(size);
	free(p);
}
void ImageData::operator delete[](void *p, size_t size) {
	MemoryMetric::get_instance().release_and_print(size);
	free(p);
}

#endif