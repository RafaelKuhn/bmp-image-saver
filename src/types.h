#pragma once


typedef unsigned int uint;
typedef unsigned char uchar;


struct Color {
	char r{0}, g{0}, b{0};
	Color(): r(0), g(0), b(0) { }
	Color(int r, int g, int b): r(r), g(g), b(b) { }
	Color(char r, char g, char b): r(r), g(g), b(b) { }
	Color(char *little_endian_3byte_bgr_arr) {
		uint* int_ptr = reinterpret_cast<uint *>(little_endian_3byte_bgr_arr);
		r = (char)(*int_ptr >> 8 * 2);
		g = (char)(*int_ptr >> 8 * 1);
		b = (char)(*int_ptr >> 8 * 0);
	};
};


template <typename T>
struct Point {
	T x;
	T y;
};


class ImageData {
private:
	uint _width;
	uint _height;

public:
	Color* colors;
	
	ImageData() {}

	ImageData(uint width, uint height): _width(width), _height(height) {
		colors = new Color[width * height];
#ifdef DEBUG
		std::cout << "[]image data created, w: " << width << " h: " << height << "\n";
#endif
	}

	~ImageData() {
#ifdef DEBUG
		std::cout << "[]image data destroyed \n";
#endif
		delete[] colors;
	}

	constexpr uint height() const {	return _height; }
	constexpr uint width() const { return _width; }
};