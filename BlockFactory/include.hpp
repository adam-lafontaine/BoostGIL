#pragma once
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // !_CRT_SECURE_NO_WARNINGS

#include <boost/gil.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/jpeg/old.hpp>

#include <functional>
#include <vector>
#include <array>
#include <algorithm>
#include <string>

namespace gil = boost::gil;

using bits8 = uint8_t;
using bits32 = uint32_t;

// png types
using image_t = gil::rgba8_image_t;
using ref_t = gil::rgba8_ref_t;
using pixel_t = image_t::value_type; // rgba8_pixel_t
using pixel_ptr_t = gil::rgba8_ptr_t;
using view_t = gil::rgba8_view_t;
using pixel_ptr_list_t = std::vector<pixel_ptr_t>;
using index_t = std::ptrdiff_t;

using pix_trans_cb_t = std::function<pixel_t(ref_t const&)>;


typedef struct
{
	index_t x_begin;
	index_t x_end;
	index_t y_begin;
	index_t y_end;
} pixel_range_t;

using range_list_t = std::vector<pixel_range_t>;



typedef struct rgb8color_t
{
	bits8 r;
	bits8 g;
	bits8 b;
}color;

constexpr bits32 to_bits32(bits8 red, bits8 green, bits8 blue)
{
	const bits32 r = red;
	const bits32 g = green;
	const bits32 b = blue;

	return (r << 16) + (g << 8) + b;
}

constexpr bits32 BLACK32 = 0;
constexpr bits32 RED32 = to_bits32(255, 0, 0);
constexpr bits32 GREEN32 = to_bits32(0, 255, 0);
constexpr bits32 BLUE32 = to_bits32(0, 0, 255);
constexpr bits32 WHITE32 = to_bits32(255, 255, 255);

constexpr bits32 SRC_EDGE_COLOR = BLACK32;
constexpr bits32 SRC_LETTER_COLOR = BLUE32;
constexpr bits32 SRC_SURFACE_COLOR = GREEN32;
constexpr bits32 SRC_BACKGROUND_COLOR = WHITE32;

constexpr color GREY = { 195, 195, 195 };
constexpr color BLACK = { 0, 0, 0 };
constexpr color WHITE = { 255, 255, 255 };

constexpr color ORANGE = { 255, 128, 0 };
constexpr color ORANGE_1 = { 200, 100, 0 };

constexpr color PURPLE = { 163, 73, 164 };
constexpr color PURPLE_1 = { 150, 73, 180 };

constexpr color YELLOW = { 170, 170, 0 };
constexpr color YELLOW_1 = { 150, 150, 0 };

constexpr color RED = { 180, 0, 0 };
constexpr color RED_1 = { 90, 0, 0 };

constexpr color BLUE = { 0, 64, 128 };
constexpr color BLUE_1 = { 0, 30, 170 };

constexpr color GREEN = { 0, 128, 0 };
constexpr color GREEN_1 = { 10, 220, 10 };


//constexpr std::array<color, 3> LETTER_COLORS = { ORANGE, PURPLE, YELLOW };
//constexpr std::array<color, 3> SURFACE_COLRS = { RED, BLUE, GREEN };


constexpr std::array<color, 6> LETTER_COLORS = { ORANGE, PURPLE, YELLOW, ORANGE_1, PURPLE_1, YELLOW_1 };
constexpr std::array<color, 6> SURFACE_COLRS = { RED, BLUE, GREEN, RED_1, BLUE_1, GREEN_1 };

// converts pixel reference to a 32 bit integer
bits32 to_bits32(ref_t const& src)
{
	gil::red_t constexpr   R;
	gil::green_t constexpr G;
	gil::blue_t constexpr  B;
	//gil::alpha_t constexpr A;

	auto const src_r = gil::get_color(src, R);
	auto const src_g = gil::get_color(src, G);
	auto const src_b = gil::get_color(src, B);

	return to_bits32(src_r, src_g, src_b);
}


// converts a pointer to a pixel to a 32 bit integer
bits32 to_bits32(pixel_ptr_t const& ptr)
{
	gil::red_t constexpr   R;
	gil::green_t constexpr G;
	gil::blue_t constexpr  B;
	//gil::alpha_t constexpr A;

	auto const src_r = gil::get_color(*ptr, R);
	auto const src_g = gil::get_color(*ptr, G);
	auto const src_b = gil::get_color(*ptr, B);

	return to_bits32(src_r, src_g, src_b);
}

// creates a pixel object from color values
pixel_t to_pixel(color const& c)
{
	return pixel_t(c.r, c.g, c.b, 255);
}

// gets pixels from a single column
pixel_ptr_list_t to_pixel_column(view_t const& img_v, index_t x)
{
	const auto height = img_v.height();

	pixel_ptr_list_t list;
	list.reserve(height);

	for (index_t y = 0; y < height; ++y)
	{
		list.push_back(img_v.row_begin(y) + x);
	}

	return list;
}


// gets pixels from a single row
pixel_ptr_list_t to_pixel_row(view_t const& img_v, index_t x_begin, index_t x_end, index_t y)
{
	pixel_ptr_list_t list;
	list.reserve(x_end - x_begin);

	for (index_t x = x_begin; x < x_end; ++x)
	{
		list.push_back(img_v.row_begin(y) + x);
	}

	return list;
}


//======= READING IMAGES =====================


image_t read_image_png(const char* img_path)
{
	gil::image_read_settings<gil::png_tag> read_settings;
	image_t src_img;
	gil::read_image(img_path, src_img, /*read_settings*/gil::png_tag());

	return src_img;
}


gil::rgb8_image_t read_image_jpeg(const char* img_path)
{
	gil::image_read_settings<gil::jpeg_tag> read_settings;
	gil::rgb8_image_t src_img;
	gil::read_image(img_path, src_img, read_settings);

	return src_img;
}



// choose png or jpeg
template<typename path_t>
image_t read_image_from_file(path_t img_path)
{
	return read_image_png(img_path);
}



//======= WRITE IMAGES =====================

template<typename String>
void write_image_view_png(String const& file_path, view_t const& view)
{
	gil::write_view(file_path, view, gil::png_tag());
}


template<typename String>
void write_image_view_jpeg(String const& file_path, view_t const& view)
{	
	gil::jpeg_write_view(file_path, view);

	//gil::write_view(file_path, view, gil::jpeg_tag());
}


// choose jpeg or png
template<typename String>
void write_image_view(String const& file_path, view_t const& view)
{
	write_image_view_png(file_path, view);
}

//======= OTHER HELPERS ====================

// checks if a string ends with another string
bool str_ends_with(std::string const& full_string, std::string const& end) {

	return end.length() <= full_string.length() &&
		full_string.compare(full_string.length() - end.length(), end.length(), end) == 0;

}

// appends sub file/directory to a directory path string
std::string str_append_sub(std::string const& parent_dir, std::string const& sub) {
	constexpr auto back_slash = R"(\)";
	constexpr auto forward_slash = R"(/)";

	return str_ends_with(parent_dir, back_slash) || str_ends_with(parent_dir, forward_slash) ?
		parent_dir + sub : parent_dir + forward_slash + sub;
}

// gets the number of digits in an unsigned int
size_t num_digits(unsigned const number)
{
	// int overflow?

	char buffer[100];
	sprintf_s(buffer, "%d", (int)number); // std::to_string(number).length() dum dum
	return strlen(buffer);
}

// gets the number of digits in an unsigned int
size_t num_digits(unsigned long long const number)
{
	// int overflow?

	char buffer[100];
	sprintf_s(buffer, "%d", (int)number); // std::to_string(number).length() dum dum
	return strlen(buffer);
}