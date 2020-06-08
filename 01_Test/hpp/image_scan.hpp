#pragma once

#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <fstream>

namespace gil = boost::gil;

//void fill_histogram(gil::rgb8_view_t const& img_v);
//void write_histogram(const char* file_path);

gil::rgb8_view_t read_image_jpg(const char* file_path) {

	// read image
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(file_path, image, read_settings);

	//fill_histogram(gil::view(image));
	//write_histogram("./out_files/multi_hist.txt");

	// to view
	return gil::view(image);
}

gil::rgb8_view_t read_image_png(char* file_path) {
	// read image
	gil::image_read_settings<gil::png_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(file_path, image, read_settings);

	// to view
	return gil::view(image);
}

//uint8_t from_24_to_6_bit(uint8_t c8r, uint8_t c8g, uint8_t c8b) {
//	// convert from 24bit to 6bit
//	unsigned cr = c8r * 3 / 255; // 8bit to 2bit
//	unsigned cg = c8g * 3 / 255;
//	unsigned cb = c8b * 3 / 255;
//
//	return uint8_t((cr << 4) + (cg << 2) + cb);
//}

uint16_t from_24_to_9_bit(uint8_t red, uint8_t green, uint8_t blue) {
	// convert from 24bit to 9bit
	unsigned cr = red * 7 / 255; // 8bit to 3bit
	unsigned cg = green * 7 / 255;
	unsigned cb = blue * 7 / 255;

	return uint16_t((cr << 6) + (cg << 3) + cb);
}





//                       a  b  c  d
unsigned pixel_histogram[4][4][4][4][512] = { 0 };

void fill_histogram(gil::rgb8_view_t const& img_v) {

	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	uint8_t d = 0;

	bool a_half_h;
	bool b_half_h;
	bool c_half_h;
	bool d_half_h;

	bool a_half_w;
	bool b_half_w;
	bool c_half_w;
	bool d_half_w;

	unsigned width = img_v.width();
	unsigned height = img_v.height();

	// std::midpoint C++20

	for (size_t y = 0; y < height; ++y) {
		a_half_h = y >= height / 2; // past halfway of height
		b_half_h = (y - a_half_h * height / 2) >= height / 4;
		c_half_h = (y - a_half_h * height / 2 - b_half_h * height / 4) >= height / 8;
		d_half_h = (y - a_half_h * height / 2 - b_half_h * height / 4 - c_half_h * height / 8) >= height / 16;

		auto x_begin = img_v.row_begin(y);

		for (size_t x = 0; x < width; ++x) {
			a_half_w = x >= width / 2; // past halfway of width
			b_half_w = (x - a_half_w * width / 2) >= width / 4;
			c_half_w = (x - a_half_w * width / 2 - b_half_w * width / 4) >= width / 8;
			d_half_w = (x - a_half_w * width / 2 - b_half_w * width / 4 - c_half_w * width / 8) >= width / 16;

			a = (a_half_w && !a_half_h) + 2 * (!a_half_w && a_half_h) + 3 * (a_half_w && a_half_h);
			b = (b_half_w && !b_half_h) + 2 * (!b_half_w && b_half_h) + 3 * (b_half_w && b_half_h);
			c = (c_half_w && !c_half_h) + 2 * (!c_half_w && c_half_h) + 3 * (c_half_w && c_half_h);
			d = (d_half_w && !d_half_h) + 2 * (!d_half_w && d_half_h) + 3 * (d_half_w && d_half_h);

			auto x_it = x_begin + x;
			auto pixel = *x_it;
			//auto pixel = *(x_begin + x);

			auto c8r = pixel[0]; // auto red = gil::get_color(*x_it, gil::red_t());
			auto c8g = pixel[1]; // auto green = gil::get_color(*x_it, gil::green_t());
			auto c8b = pixel[2]; // auto blue = gil::get_color(*x_it, gil::blue_t());			

			// 6bit color => 0 - 63, 9bit => 511
			auto converted = from_24_to_9_bit(c8r, c8g, c8b);

			++pixel_histogram[a][b][c][d][converted];
		}

	}


}

void write_histogram(char* const file_path) {
	
	std::fstream histo_file(file_path, std::ios::out);

	for (size_t ia = 0; ia < 4; ++ia) {
		for (size_t ib = 0; ib < 1; ++ib) {
			for (size_t ic = 0; ic < 1; ++ic) {
				for (size_t id = 0; id < 1; ++id) {
					histo_file << "hist[" << ia << "]" << "[" << ib << "]" << "[" << ic << "]" << "[" << id << "]" << "\n";
					histo_file << "==================\n";
					for (size_t ih = 0; ih < 512; ++ih) {
						histo_file << ih << " => " << pixel_histogram[ia][ib][ic][id][ih] << "\n";
					}

					histo_file << std::endl;
				}
			}
		}
	}
	histo_file.close();

}

void scan_image(char* const file_path) {

	// read image
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(file_path, image, read_settings);

}