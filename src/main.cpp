#include <iostream>
#include <fstream> // ofstream

#include <string> // string, stringstream
#include <cmath> // pow

#include <direct.h> // windows' mkdir
#include <memory> // unique_ptr

using std::cout;
using std::string;
using std::unique_ptr;
using std::ofstream;
using std::ios_base;

struct Color {
	unsigned short r{0}, g{0}, b{0};
	
	Color(): r(0), g(0), b(0) { }
	Color(int r, int g, int b): r(r), g(g), b(b) { }
};

template <typename T>
struct Point {
	T x;
	T y;
};

void draw_uv_gradient(unique_ptr<Color[]> &data, int width, int height)
{
	std::cout << "drawing uv gradient on canvas with width " << width << ", height " << height << "\n\n";

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			data[y * width + x] = { (x * 255)/(width-1), (y * 255)/(height-1), 0 };
		}
	}
}

void draw_circle(Point<int> center, int radius, int width, int height, unique_ptr<Color[]> &output)
{
	std::cout << "drawing circle with radius " << radius << ", center at " << center.x << "," << center.y << "\n\n";
	std::cout << "on canvas with width " << width << ", height " << height << "\n";
	
	float radiusf = (float)radius;

	float i, j;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// by summing 0.5, we calculate the distance between the center of the pixel, not the top left corner of it
			i = x + 0.5f - center.x;
			j = y + 0.5f - center.y;
			if (pow(i, 2.0f) + pow(j, 2.0f) < pow(radiusf, 2.0f))
				output[y * width + x] = { 255, 0, 0 };

			else
				output[y * width + x] = { 0, 0, 0 };
		}
	}
}

void write_as_ppm(const char *file_name, Color *data, int width, int height)
{
	string folder_name = string("ppm");
	mkdir(folder_name.data());

	string path = folder_name.append("/").append(file_name);

	ofstream file(path, ios_base::binary);

	file << "P6" << "\n"; // 2 bytes identifier + 1 byte line feed ("\n" in ascii: (char)10)
	file << std::to_string(width) << " " << std::to_string(height) << "\n"; // integer_width + space + integer_height  + line feed
	file << std::to_string(255) << "\n"; // integer_blocksize (255) + line feed

	// bits of colors, raster graphics
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Color color = data[y * width + x];
			file << (char)(color.r) << (char)(color.g) << (char)(color.b);
		}
	}

	std::cout << "data saved to \"" << path.data() << "\", pointer to data: " << data << "\n" << "\n";
	file.close();
}

void truncate_to_4_byte_chars_lsb(int number, unsigned char *output)
{
	output[0] = (unsigned char)(number >> 8 * 0);
	output[1] = (unsigned char)(number >> 8 * 1);
	output[2] = (unsigned char)(number >> 8 * 2);
	output[3] = (unsigned char)(number >> 8 * 3);
}

void write_as_bmp(const char* file_name, unique_ptr<Color[]> &data, int width, int height)
{
	string folder_name = string("bmp");
	mkdir(folder_name.data());

	string path = folder_name.append("/").append(file_name);

	ofstream file(path.data(), ios_base::binary);

	const char zero = (char)0;

	// header: (3 rows * 16 bytes per row) + 6 bytes last row, 16 * 3 + 6 = 54
	const int header_size = 54;

	const int pads_len_per_row = (4 - ( ( width * 3) % 4) ) % 4;
	const int pads_len = pads_len_per_row * height;

	const int img_size = width * height * 3 + pads_len;
	const int file_size = header_size + img_size;

	const char zeros[4] { (char)0, (char)0, (char)0, (char)0 };
	unsigned char truncated_data[4] { (char)0, (char)0, (char)0, (char)0 };

	file << "BM"; // 2 bytes of identifier

	truncate_to_4_byte_chars_lsb(file_size, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes of file size (bits)

	file.write(zeros, 4); // 4 bytes reserved
	file << (char)54 << zero << zero << zero; // 4 bytes offset image data (header size)
	file << (char)40 << zero << zero << zero; // 4 bytes DIB header size
	
	truncate_to_4_byte_chars_lsb(width, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes pixel width
	truncate_to_4_byte_chars_lsb(height, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes pixel height

	file << (char)1 << zero << (char)24 << zero; // 2 bytes amount of planes (must be 1) + 2 bytes pixel density (color depth)
	file.write(zeros, 4); // 4 bytes compression (0 for none)

	truncate_to_4_byte_chars_lsb(img_size, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3];; // 4 bytes img size in bytes

	// printing data, could be all zeros, here is 2835 because yes ( 72 DPI × 39.3701 meters per inch = 2835 )
	file << (char)19 << (char)11 << zero << zero; // 4 bytes printing data, x_pix_per_meter
	file << (char)19 << (char)11 << zero << zero; // 4 bytes printing data, y_pix_per_meter

	file.write(zeros, 4); // 4 bytes colors in color table
	file.write(zeros, 4); // 4 bytes important colors ??

	std::cout << "width " << width << ", height " << height << ", bytes per row " << width*3 << "\n";
	std::cout << "padding per row " << pads_len_per_row << " bytes, " <<  "\n";
	std::cout << "img size " << img_size << " bytes" << "\n";
	std::cout << "header size " << header_size << " bytes" << "\n";
	std::cout << "file size " << header_size << " + " << img_size << " = " << file_size << " bytes" << "\n";

	for (unsigned int y = 0; y < height; ++y) {
		unsigned int y_reversed = height - y - 1;

		for (unsigned int x = 0; x < width; ++x) {
			int index = y_reversed * width + x;
			Color color = data[index];
			file << (char)color.b << (char)color.g << (char)color.r;
			// std::cout << "[" << color.r << "," << color.g << "," << color.b << "] "; // debug
		}

		for (int i = 0; i < pads_len_per_row; ++i) {
			file << zero;
		}
		// std::cout << "\n";
	}

	std::cout << "data saved to \"" << path.data() << "\", pointer to data: " << data.get() << "\n" << "\n";
	file.close();
}

void print_color_data_by_ref(unique_ptr<Color[]> &data, int width, int height)
{
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Color el = data[y * width + x];
			std::cout << "[" << el.r << "," << el.g << "," << el.b << "] ";
		}
		std::cout << "\n";
	}
}

int main(int argc, char** argv)
{
	int width;
	int height;
	
	height = 255;
	width  = 255;
	{
		unique_ptr<Color[]> arr = unique_ptr<Color[]>(new Color[height * width]);
		
		draw_uv_gradient(arr, width, height);
		
		// print_color_data_by_ref(arr, width, height);
		write_as_bmp("gradient.bmp", arr, width, height);
	}
	
	height = 144;
	width  = 256;
	{
		unique_ptr<Color[]> data = unique_ptr<Color[]>(new Color[height * width]);

		Point<int> center = { .x = width/2, .y = height/2 };
		int radius = height/2;
		draw_circle(center, radius, width, height, data);
		
		// print_color_data_by_ref(arr, width, height);
		write_as_bmp("circle.bmp", data, width, height);
	}
}