#include <iostream>    // cout
#include <fstream>     // ofstream
#include <memory>      // unique_ptr, move?

#include <string>      // string, stringstream, to_string

#include "bmp-types.h" // Color, Point, ImageData
#include "bmp.h"       // read_as_bmp, write_as_bmp

using std::cout;


template <typename T>
struct Point {
	T x;
	T y;
};



// use this to print a color directly, via "cout << color;"
#ifdef DEBUG_MODE
std::ostream &operator << (std::ostream &out, const Color &col)
{
	out << "[" << (uint)(uchar)col.r << "," << (uint)(uchar)col.g << "," << (uint)(uchar)col.b << "]";
	return out;
}
#endif


void draw_uv_gradient(Color* output, int width, int height)
{
#ifdef DEBUG_MODE
	cout << "drawing uv gradient on canvas with width " << width << ", height " << height << "\n\n";
#endif
	int _height = (height == 1) ? 2 : height;
	int _width = (width == 1) ? 2 : width;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			output[y * width + x] = { (x * 255)/(_width-1), (y * 255)/(_height-1), 0 };
		}
	}
}

void draw_circle(Color* output, const Point<int> center, int radius, int width, int height)
{
#ifdef DEBUG_MODE
	cout << "drawing circle with radius " << radius << ", center at " << center.x << "," << center.y << "\n\n";
	cout << "on canvas with width " << width << ", height " << height << "\n";
#endif
	float radiusf = (float)radius;

	float i, j;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// by summing 0.5, we calculate the distance from the center of the pixel, not the top left corner of it
			i = x + 0.5f - center.x;
			j = y + 0.5f - center.y;
			
			// x^2 + y^2 > radius^2
			if (i*i + j*j < radiusf*radiusf)
				output[y * width + x] = { 255, 0, 0 };

			else
				output[y * width + x] = { 0, 0, 0 };
		}
	}
}

int main(int, char*[])
{	
	int width = 426;
	int height = 240;

	cout << "[test] creating a 240p uv gradient and writing it into \"gradient.bmp\"\n";
	{
		std::unique_ptr<Color[]> arr = std::make_unique<Color[]>(height * width);

		draw_uv_gradient(arr.get(), width, height);
		write_as_bmp("gradient.bmp", arr.get(), width, height);
	}

	cout << "[test] reading the gradient from disc and re-writing it as \"gradient-copy.bmp\"\n";
	{
		std::unique_ptr<ImageData> data = read_as_bmp("gradiente.bmp");

		// could do smth with the data here

		if (data) {
			write_as_bmp("gradient-copy.bmp", *data);
		}
	}

	width  = 256;
	height = 144;

	cout << "[test] creating a 144p circle and writing it into \"circle.bmp\"\n";
	{
		auto img_data = std::make_unique<ImageData>(width, height);
		
		Point<int> center = { width/2, height/2 };
		int radius = height/2;
		draw_circle(img_data->colors, center, radius, img_data->get_width(), img_data->get_height());

		write_as_bmp("circle.bmp", img_data->colors, width, height);
	}
	
	// std::cin.get();
	cout << "\n[test] test program ended! big success!\n";
}