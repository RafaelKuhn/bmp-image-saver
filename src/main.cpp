// this doesnt work on Big-endian IBM mainframes lol

#include <iostream>
#include <fstream> // ofstream
#include <memory> // unique_ptr
#include <type_traits> // move

#include <string> // string, stringstream
#include <cmath> // pow

#include <direct.h> // windows' mkdir

// SHORTHAND TYPES
typedef unsigned int uint;
typedef unsigned char uchar;

// COLOR
struct Color {
	char r{0}, g{0}, b{0};
	
	Color(): r(0), g(0), b(0) { }
	Color(int r, int g, int b): r(r), g(g), b(b) { }
	Color(char r, char g, char b): r(r), g(g), b(b) { }
	Color(char *bgr_colors_arr) {
		uint* int_ptr = reinterpret_cast<uint *>(bgr_colors_arr);
		// file format supposed to be Little-endian
		r = (char)(*int_ptr >> 8 * 2);
		g = (char)(*int_ptr >> 8 * 1);
		b = (char)(*int_ptr >> 8 * 0);
	}
};

std::ostream &operator << (std::ostream &out, const Color &col)
{
	out << "[" << (uint)(uchar)col.r << "," << (uint)(uchar)col.g << "," << (uint)(uchar)col.b << "]";
	return out;
}

// POINT
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

	ImageData(uint width, uint height): _width(width), _height(height)
	{
		colors = new Color[width * height];
		std::cout << "[]image data created, w: " << width << " h: " << height << "\n";
	}

	~ImageData()
	{
		std::cout << "[]image data destroyed \n";
		delete[] colors;
	}

	constexpr uint height() const {	return _height; }
	constexpr uint width() const { return _width; }
};

// TODO: this throws error when height is 1
void draw_uv_gradient(Color* output, int width, int height)
{
	std::cout << "drawing uv gradient on canvas with width " << width << ", height " << height << "\n\n";

	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			output[y * width + x] = { (x * 255)/(width-1), (y * 255)/(height-1), 0 };
		}
	}
}

void draw_circle(Color* &output, Point<int> center, int radius, int width, int height)
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
	std::string folder_name = std::string("ppm");
	mkdir(folder_name.data());

	std::string path = folder_name.append("/").append(file_name);

	std::ofstream file(path, std::ios_base::binary);

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

	std::cout << "data saved to \"" << path.data() << "\"\n\n";
	file.close();
}

void truncate_to_4_byte_little_endian_char_array(int number, unsigned char *output)
{
	output[0] = (unsigned char)(number >> 8 * 0);
	output[1] = (unsigned char)(number >> 8 * 1);
	output[2] = (unsigned char)(number >> 8 * 2);
	output[3] = (unsigned char)(number >> 8 * 3);
}

void write_as_bmp(const char* file_name, Color* data, int width, int height)
{
	std::string folder_name = std::string("bmp");
	mkdir(folder_name.data());

	std::string path = folder_name.append("/").append(file_name);

	std::ofstream file(path.data(), std::ios_base::binary);

	if (!file) {
		std::cout << "file " << file_name << " could not be created!\n";
		return;
	}

	const char zero = (char)0;

	// header: (3 rows * 16 bytes per row) + 6 bytes last row, 16 * 3 + 6 = 54
	const int header_size = 54;

	const int pads_len_per_row = (4 - ( ( width * 3) % 4) ) % 4;
	const int pads_len = pads_len_per_row * height;

	const int img_size = width * height * 3 + pads_len;
	const int file_size = header_size + img_size;

	const char zeros[4] { (char)0, (char)0, (char)0, (char)0 };

	file << "BM"; // 2 bytes of identifier

	unsigned char truncated_data[4] { (char)0, (char)0, (char)0, (char)0 };

	truncate_to_4_byte_little_endian_char_array(file_size, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes of file size (bits)

	file.write(zeros, 4); // 4 bytes reserved
	file << (char)54 << zero << zero << zero; // 4 bytes offset image data (header size)
	file << (char)40 << zero << zero << zero; // 4 bytes DIB header size
	
	truncate_to_4_byte_little_endian_char_array(width, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes pixel width
	truncate_to_4_byte_little_endian_char_array(height, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes pixel height

	file << (char)1 << zero << (char)24 << zero; // 2 bytes img planes (must be 1) + 2 bytes pixel density (color depth)
	file.write(zeros, 4); // 4 bytes compression (0 for none)

	truncate_to_4_byte_little_endian_char_array(img_size, truncated_data);
	file << truncated_data[0] << truncated_data[1] << truncated_data[2] << truncated_data[3]; // 4 bytes img size in bytes

	// printing data, could be all zeros, here is 2835 because yes ( 72 DPI × 39.3701 meters per inch = 2835 )
	file << (char)19 << (char)11 << zero << zero; // 4 bytes printing data, x_pix_per_meter
	file << (char)19 << (char)11 << zero << zero; // 4 bytes printing data, y_pix_per_meter

	file.write(zeros, 4); // 4 bytes colors in color table
	file.write(zeros, 4); // 4 bytes important colors ??


	std::cout << "writing to \"" << path.data() << "\"\n";
	std::cout << "width " << width << ", height " << height << ", bytes per row " << width*3 << "\n";
	std::cout << "padding per row " << pads_len_per_row << " bytes, " <<  "\n";
	std::cout << "img size " << img_size << " bytes, header size " << header_size << " bytes \n";
	std::cout << "file size " << header_size << " + " << img_size << " = " << file_size << " bytes\n\n";

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			int index = y * width + x;
			Color color = data[index];
			file << (char)color.b << (char)color.g << (char)color.r;
		}

		for (int i = 0; i < pads_len_per_row; ++i) {
			file << zero;
		}
	}

	
	file.close();
}

void write_as_bmp(const char* file_name, std::unique_ptr<ImageData> &img_data)
{
	write_as_bmp(file_name, img_data->colors, img_data->width(), img_data->height());
}

std::unique_ptr<ImageData> read_as_bmp(const char *file_name)
{
	std::string folder_name = std::string("bmp");
	mkdir(folder_name.data());
	
	std::string path = folder_name.append("/").append(file_name);

	std::ifstream file(path.data(), std::ios_base::binary);

	if (!file) {
		std::cout << "error: file " << path << " not found!\n";
		return nullptr;
	}

	// skip to get offset of pixel data
	file.seekg(10);
	uint offset_px_data;
	// this passes pointer to .read as if it were a char pointer
	file.read(reinterpret_cast<char *>(&offset_px_data), sizeof(offset_px_data));

	uint width;
	file.seekg(18); // width file bit location
	file.read(reinterpret_cast<char *>(&width), sizeof(width));
	

	uint height;
	file.seekg(22); // height file bit location
	file.read(reinterpret_cast<char *>(&height), sizeof(height));

	// TODO: comment here
	const int pads_len_per_row = (4 - ( ( width * 3) % 4) ) % 4;

	std::cout << "reading from \"" << file_name << "\"\n";
	std::cout << "offset px data: " << offset_px_data << ", width px: " << width << ", height px: " << height << "\n\n";

	std::unique_ptr<ImageData> img_data = std::make_unique<ImageData>(width, height);
	
	for (uint y = 0; y < img_data->height(); ++y) {

		// seeks start of image data in file
		int byte_seek_position = offset_px_data + y * width * 3 + pads_len_per_row * y;
		file.seekg(byte_seek_position);

		for (uint x = 0; x < img_data->width(); ++x) {	

			// read 3 bytes (B G R color components)
			char color_data[3];
			file.read(color_data, 3);
			Color col(color_data);
			img_data->colors[y * width + x] = col;
		}
	}

	file.close();
	return std::move(img_data);
}

void print_color_data_by_ref(std::unique_ptr<Color[]> &data, int width, int height)
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
	int width = 426;
	int height = 240;
	
	// create a 240p uv gradient and write it into an image file (bmp/gradient.bmp)
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
			// write_as_bmp("renpa-copy.bmp", data->colors, data->width(), data->height());
			write_as_bmp("gradient-copy.bmp", data);
		}
	}

	width  = 256;
	height = 144;

	// create a 144p circle and write it into an image file
	{
		auto img_data = std::make_unique<ImageData>(width, height);
		
		Point<int> center = { .x = width/2, .y = height/2 };
		int radius = height/2;
		draw_circle(img_data->colors, center, radius, img_data->width(), img_data->height());
		
		write_as_bmp("circle.bmp", img_data->colors, width, height);
	}

	std::cout << "\nprogram ended! big success!\n";
}