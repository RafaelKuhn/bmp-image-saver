// TODO: allow reading of alpha values
#include "bmp.h"
#include "types.h"

#include <iostream>  // cout
#include <fstream>   // ofstream

#ifdef DEBUG_MODE
	using std::cout;
#endif

void truncate_to_4_byte_little_endian_char_array(int number, unsigned char *output)
{
	output[0] = (unsigned char)(number >> 8 * 0);
	output[1] = (unsigned char)(number >> 8 * 1);
	output[2] = (unsigned char)(number >> 8 * 2);
	output[3] = (unsigned char)(number >> 8 * 3);
}

void write_as_bmp(const char* file_name, const ImageData &img_data)
{
	write_as_bmp(file_name, img_data.colors, img_data.get_width(), img_data.get_height());
}

void write_as_bmp(const char* path, const Color* const data, uint width, uint height)
{
	std::ofstream file(path, std::ios_base::binary);
	
	if (!file) {
		std::cerr << "[bmp error] file " << path << " could not be created!\n";
		return;
	}

	const char zero = (char)0;

	// header: (3 rows * 16 bytes per row) + 6 bytes last row, 16 * 3 + 6 = 54
	const uint header_size = 54;

	const uint pads_len_per_row = (4 - ( ( width * 3) % 4) ) % 4;
	const uint pads_len = pads_len_per_row * height;

	const uint img_size = width * height * 3 + pads_len;
	const uint file_size = header_size + img_size;

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

#ifdef DEBUG_MODE
	cout << "writing bmp file to \"" << path << "\"\n";
	cout << "width " << width << ", height " << height << "\n";
	cout << "bytes per row: " << width*3 << ", padding per row: " << pads_len_per_row << " bytes, " <<  "\n";
	cout << "img size " << img_size << " bytes, header size " << header_size << " bytes \n";
	cout << "file size " << header_size << " + " << img_size << " = " << file_size << " B | " << file_size/(float)1024 << " KB\n";
	cout << "\n";
#endif

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			// bmp is retarded and stores pixel data bottom-to-top
			// so we have to do "(height - y - 1)" to invert y
			int index = (height - y - 1) * width + x;
			Color color = data[index];
			file << (char)color.b << (char)color.g << (char)color.r;
		}

		for (uint32_t i = 0; i < pads_len_per_row; ++i) {
			file << zero;
		}
	}
	
	file.close();
}


std::unique_ptr<ImageData> read_as_bmp(const char *file_name)
{
	std::ifstream file(file_name, std::ios_base::binary);

	if (!file) {
		std::cerr << "[bmp error] file " << file_name << " not found! returning nullptr\n";
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
#ifdef DEBUG_MODE
	cout << "reading bmp file from \"" << file_name << "\"\n";
	cout << "offset px data: " << offset_px_data << ", width px: " << width << ", height px: " << height << "\n";
	cout << "\n";
#endif

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
			// bmp is retarded and stores pixel data bottom-to-top
			// so we have to do "(height - y - 1)" to invert y
			img_data->colors[(height - y - 1) * width + x] = col;
		}
	}

	file.close();
	
	// implicit std::move
	return img_data;
}