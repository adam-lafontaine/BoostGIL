#pragma once

#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <iostream>

namespace gil = boost::gil;

constexpr auto KITTEN_JPG = "./media/kitten-bowtie-400x400.jpg";

void read_image() {		

	try
	{
		gil::image_read_settings<gil::jpeg_tag> read_settings;
		gil::rgb8_image_t image;
		gil::read_image(KITTEN_JPG, image, read_settings);


		std::cout << "Image read success\n";
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

//---------------------------------

void read_write_image() {

	auto IMG_PATH = "./media/kitten-write.jpg";

	try
	{
		// read image
		gil::image_read_settings<gil::jpeg_tag> read_settings;
		gil::rgb8_image_t image;
		gil::read_image(KITTEN_JPG, image, read_settings);
		std::cout << "Image read success\n";

		// write image
		gil::write_view(IMG_PATH
			, gil::view(image)
			, gil::jpeg_tag()
		);
		std::cout << "Image write success -> " << IMG_PATH << std::endl;
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

//------------------------------------

#include <algorithm>
#include <fstream>

template <typename GrayView, typename R>
void gray_image_hist(const GrayView& img_view, R& hist) {
	//    for_each_pixel(img_view,++lambda::var(hist)[lambda::_1]);
	for (typename GrayView::iterator it = img_view.begin(); it != img_view.end(); ++it)
		++hist[*it];
}

template <typename V, typename R>
void get_hist(const V & img_view, R & hist) {
	gray_image_hist(gil::color_converted_view<gil::gray8_pixel_t>(img_view), hist);
}



void histogram() {

	auto out_file = "./out_files/histogram.txt";

	// read image
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(KITTEN_JPG, image, read_settings);

	int histogram[256];
	std::fill(histogram, histogram + 256, 0);
	get_hist(gil::const_view(image), histogram);

	std::fstream histo_file(out_file, std::ios::out);
	for (std::size_t ii = 0; ii < 256; ++ii)
		histo_file << histogram[ii] << std::endl;
	histo_file.close();

}

void x_gradient(gil::gray8c_view_t const& src, gil::gray8s_view_t const& dst)
{
	for (int y = 0; y < src.height(); ++y)
	{
		gil::gray8c_view_t::x_iterator src_it = src.row_begin(y);
		gil::gray8s_view_t::x_iterator dst_it = dst.row_begin(y);

		for (int x = 1; x < src.width() - 1; ++x)
			dst_it[x] = (src_it[x - 1] - src_it[x + 1]) / 2;
	}
}

void iterate_2d() {

	// read image
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(KITTEN_JPG, image, read_settings);

	unsigned long long count = 0;

	std::cout << "Counting...\n";

	for (size_t y = 0; y < image.height(); ++y) {

		for (size_t x = 0; x < image.width(); ++x) {
			++count;
		}

	}

	std::cout << "count = " << count << std::endl;

	

}

void my_histogram() {

	// read image
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(KITTEN_JPG, image, read_settings);

	// convert to gray image
	auto gray_img = gil::color_converted_view<gil::gray8_pixel_t>(gil::const_view(image));

	unsigned hist[256] = { 0 };

	for (size_t y = 0; y < gray_img.height(); ++y) {

		for (auto x_it = gray_img.row_begin(y); x_it != gray_img.row_end(y); ++x_it) {
			++hist[*x_it];
		}
	}

	auto out_file = "./out_files/my_hist.txt";
	std::fstream histo_file(out_file, std::ios::out);
	for (std::size_t ii = 0; ii < 256; ++ii)
		histo_file << hist[ii] << std::endl;
	histo_file.close();

}

//              a  b  c  d
unsigned hist_4[4][4][4][4][64] = { 0 };

void hist_quad() {

	std::cout << "Reading image... ";

	auto img_file_path = "./media/colors-100x100.jpg";

	// read image
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t image;
	gil::read_image(img_file_path, image, read_settings);

	// to view
	auto img_v = gil::const_view(image);

	// convert to gray image
	auto gray_v = gil::color_converted_view<gil::gray8_pixel_t>(gil::const_view(image));

	std::cout << "Done\n";


	std::cout << "Scanning Image... ";	

	

	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	uint8_t d = 0;

	unsigned width = img_v.width();
	unsigned height = img_v.height();

	for (size_t y = 0; y < height; ++y) {

		bool g_half_h = y >= height / 2; // past halfway of height

		auto x_begin = img_v.row_begin(y);

		auto x_begin_g = gray_v.row_begin(y);

		for (size_t x = 0; x < width; ++x) {
			auto x_it = x_begin + x;

			auto x_it_g = x_begin_g + x;

			auto pixel = *x_it;			

			auto c8r = pixel[0]; // auto red = gil::get_color(*x_it, gil::red_t());
			auto c8g = pixel[1]; // auto green = gil::get_color(*x_it, gil::green_t());
			auto c8b = pixel[2]; // auto blue = gil::get_color(*x_it, gil::blue_t());

			// convert from 24bit to 6bit
			unsigned c2r = c8r * 3 / 255;
			unsigned c2g = c8g * 3 / 255;
			unsigned c2b = c8b * 3 / 255;
			unsigned converted = ((c2r << 4) + (c2g << 2) + c2b);

			bool g_half_w = x >= width / 2;

			a = (g_half_w && !g_half_h) + 2 * (!g_half_w && g_half_h) + 3 * (g_half_w && g_half_h);

			++hist_4[a][b][c][d][converted];
		}
	}

	std::cout << "Done\n";


	std::cout << "Writing file... ";

	auto out_file = "./out_files/quad_hist.txt";
	std::fstream histo_file(out_file, std::ios::out);

	for (size_t ia = 0; ia < 4; ++ia) {

		histo_file << "hist[" << ia << "]" << "[" << b << "]" << "[" << c << "]" << "[" << d << "]" <<"\n";

		histo_file << "==================\n";

		for (size_t ih = 0; ih < 64; ++ih) {
			histo_file << ih << " => " << hist_4[ia][b][c][d][ih] << "\n";
		}

		histo_file << std::endl;
	}
	histo_file.close();

	std::cout << "Done\n";

	
}

void abcd() {

	try {

		std::cout << "Reading image... ";

		auto img_file_path = "./media/white-100x100.jpg";

		// read image
		gil::image_read_settings<gil::jpeg_tag> read_settings;
		gil::rgb8_image_t image;
		gil::read_image(img_file_path, image, read_settings);

		// to view
		auto img_v = gil::view(image);

		std::cout << "Done\n";


		std::cout << "Scanning Image... ";
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

		auto red_pixel = gil::rgb8_pixel_t(255, 0, 0);

		unsigned width = img_v.width();
		unsigned height = img_v.height();

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

				if (d == 1) {
					*x_it = red_pixel;
				}
					
			}

		}


		std::cout << "Done\n";

		std::cout << "Writing Image... ";

		auto out_file = "./media/target.jpg";

		// write image
		gil::write_view(out_file
			, img_v
			, gil::jpeg_tag()
		);

		std::cout << "Done\n";
	}
	catch (std::exception const& e)	{
		std::cerr << e.what() << std::endl;
	}	

}

void multi_hist() {
	try {
		std::cout << "Reading image... ";

		// read image
		gil::image_read_settings<gil::jpeg_tag> read_settings;
		gil::rgb8_image_t image;
		gil::read_image(KITTEN_JPG, image, read_settings);

		// to view
		auto img_v = gil::view(image);

		std::cout << "Done\n";


		std::cout << "Scanning Image... ";
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

		auto red_pixel = gil::rgb8_pixel_t(255, 0, 0);

		unsigned width = img_v.width();
		unsigned height = img_v.height();

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

				auto c8r = pixel[0]; // auto red = gil::get_color(*x_it, gil::red_t());
				auto c8g = pixel[1]; // auto green = gil::get_color(*x_it, gil::green_t());
				auto c8b = pixel[2]; // auto blue = gil::get_color(*x_it, gil::blue_t());

				// convert from 24bit to 6bit
				unsigned c2r = c8r * 3 / 255; // 8bit to 2bit
				unsigned c2g = c8g * 3 / 255;
				unsigned c2b = c8b * 3 / 255;
				unsigned converted = ((c2r << 4) + (c2g << 2) + c2b);

				++hist_4[a][b][c][d][converted];
			}

		}
		std::cout << "Done\n";

		std::cout << "Writing file... ";

		auto out_file = "./out_files/multi_hist.txt";
		std::fstream histo_file(out_file, std::ios::out);

		for (size_t ia = 0; ia < 1; ++ia) {
			for (size_t ib = 0; ib < 1; ++ib) {
				for (size_t ic = 0; ic < 4; ++ic) {
					for (size_t id = 0; id < 4; ++id) {
						histo_file << "hist[" << ia << "]" << "[" << ib << "]" << "[" << ic << "]" << "[" << id << "]" << "\n";
						histo_file << "==================\n";
						for (size_t ih = 0; ih < 64; ++ih) {
							histo_file << ih << " => " << hist_4[ia][b][c][d][ih] << "\n";
						}

						histo_file << std::endl;
					}
				}
			}			
		}
		histo_file.close();

		std::cout << "Done\n";

	}
	catch (std::exception const& e) {
		std::cerr << e.what() << std::endl;
	}
}