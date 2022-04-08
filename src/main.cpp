#include <iostream>  // cout
#include <fstream>   // ofstream
#include <memory>    // unique_ptr, move?
 
#include <string>    // string, stringstream, to_string

#include "types.h"   // Color, Point, ImageData
#include "bmp.h"     // read_as_bmp, write_as_bmp


using std::cout;


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

void draw_circle(Color* output, Point<int> center, int radius, int width, int height)
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

// TODO: segregate in its own file
void write_as_ppm(const char *path, Color *data, uint width, uint height)
{
	std::ofstream file(path, std::ios_base::binary);

	file << "P6" << "\n"; // 2 bytes identifier + 1 byte line feed ("\n" in ascii: (char)10)
	file << std::to_string(width) << " " << std::to_string(height) << "\n"; // integer_width + space + integer_height  + line feed
	file << std::to_string(255) << "\n"; // integer_blocksize (255) + line feed

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			Color color = data[y * width + x];
			file << (char)(color.r) << (char)(color.g) << (char)(color.b);
		}
	}
#ifdef DEBUG_MODE
	cout << "data saved to \"" << path << "\"\n\n";
#endif
	file.close();
}

// TODO: move to color implementation
#ifdef DEBUG_MODE
void print_color_data_by_ref(std::unique_ptr<Color[]> &data, int width, int height)
{
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Color el = data[y * width + x];
			cout << "[" << el.r << "," << el.g << "," << el.b << "] ";
		}
		cout << "\n";
	}
}
#endif

int main(int, char**)
{	
	int width = 426;
	int height = 240;

	// create a 240p uv gradient and write it into an image file (gradient.bmp)
	{
		std::unique_ptr<Color[]> arr = std::make_unique<Color[]>(height * width);

		draw_uv_gradient(arr.get(), width, height);
		write_as_bmp("gradient.bmp", arr.get(), width, height);
	}

	// read the gradient from disc and write it as a copy
	{
		std::unique_ptr<ImageData> data = read_as_bmp("gradient.bmp");

		// TODO: do smth with the data

		if (data) {
			write_as_bmp("gradient-copy.bmp", data);
		}
	}

	width  = 256;
	height = 144;

	// create a 144p circle and write it into an image file
	{
		auto img_data = std::make_unique<ImageData>(width, height);
		
		Point<int> center = { width/2, height/2 };
		int radius = height/2;
		draw_circle(img_data->colors, center, radius, img_data->get_width(), img_data->get_height());

		write_as_bmp("circle.bmp", img_data->colors, width, height);
	}
	
	// std::cin.get();
	cout << "\nprogram ended! big success!\n";
}