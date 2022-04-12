#include "bmp.h"

#include <iostream>  // cout
#include <fstream>   // ofstream

void truncate_to_4_byte_little_endian_char_array(int number, unsigned char *output)
{
	output[0] = (unsigned char)(number >> 8 * 0);
	output[1] = (unsigned char)(number >> 8 * 1);
	output[2] = (unsigned char)(number >> 8 * 2);
	output[3] = (unsigned char)(number >> 8 * 3);
}

void write_as_bmp(const char* path, Color* data, uint width, uint height)
{
	using std::cout;
	
	std::ofstream file(path, std::ios_base::binary);
	
	if (!file) {
		cout << "[error] file " << path << " could not be created!\n";
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

	cout << "writing to \"" << path << "\"\n";
	cout << "width " << width << ", height " << height << "\n";
#ifdef DEBUG_MODE
	cout << "bytes per row: " << width*3 << ", padding per row: " << pads_len_per_row << " bytes, " <<  "\n";
	cout << "img size " << img_size << " bytes, header size " << header_size << " bytes \n";
	cout << "file size " << header_size << " + " << img_size << " = " << file_size << " bytes\n";
#endif
	cout << "\n";

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
	write_as_bmp(file_name, img_data->colors, img_data->get_width(), img_data->get_height());
}


std::unique_ptr<ImageData> read_as_bmp(const char *file_name)
{
	using std::cout;
	
	std::ifstream file(file_name, std::ios_base::binary);

	if (!file) {
		cout << "error: file " << file_name << " not found!\n";
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

	// amount of bytes per width row must be a multiple of four
	// https://www.desmos.com/calculator/akzk5k0rrv
	const int pads_len_per_row = (4 - ( ( width * 3) % 4) ) % 4;
	cout << "reading from \"" << file_name << "\"\n";
#ifdef DEBUG_MODE
	cout << "offset px data: " << offset_px_data << ", width px: " << width << ", height px: " << height << "\n";
#endif
	cout << "\n";

	std::unique_ptr<ImageData> img_data = std::make_unique<ImageData>(width, height);
	
	for (uint y = 0; y < img_data->get_height(); ++y) {

		// seeks start of image data in file
		int byte_seek_position = offset_px_data + y * width * 3 + pads_len_per_row * y;
		file.seekg(byte_seek_position);

		for (uint x = 0; x < img_data->get_width(); ++x) {	

			// read 3 bytes (B G R color components)
			char color_data[3];
			file.read(color_data, 3);
			Color col(color_data);
			img_data->colors[y * width + x] = col;
		}
	}

	file.close();
	
	// implicit std::move
	return img_data;
}