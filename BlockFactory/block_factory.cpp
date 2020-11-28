#include "include.hpp"
#include "block_factory.hpp"

#include <random>



// parses the image and gets the pixel ranges of each letter
range_list_t get_letter_ranges(view_t const& img_v)
{
	range_list_t letter_ranges;

	const auto width = img_v.width();
	const auto height = img_v.height();

	bool found = false;
	const auto is_black_pred = [](pixel_ptr_t const& ptr) { return to_bits32(ptr) == BLACK32; };

	index_t x_begin = 0;

	// find horizontal positions of each letter
	for (index_t x = 0; x < width; ++x)
	{
		auto column = to_pixel_column(img_v, x);
		bool has_black = std::any_of(column.begin(), column.end(), is_black_pred);

		if (!found && has_black) // found letter
		{
			x_begin = x;
			found = true;
		}

		if (found && !has_black) // end of letter
		{
			pixel_range_t range;
			range.x_begin = x_begin;
			range.x_end = x;
			range.y_begin = 0;
			range.y_end = height;

			letter_ranges.push_back(range);			
			found = false;
		}
	}

	// trim white areas from top and bottom
	found = false;
	for (auto& range : letter_ranges)
	{
		for (index_t y = range.y_begin; y < range.y_end; ++y)
		{
			auto row = to_pixel_row(img_v, range.x_begin, range.x_end, y);
			bool has_black = std::any_of(row.begin(), row.end(), is_black_pred);

			if (!found && has_black)
			{
				range.y_begin = y;
				found = true;
			}

			if (found && !has_black)
			{
				range.y_end = y;
				found = false;
			}
		}
		
	}

	return letter_ranges;
}


// get pixel positions to center a smaller view inside a larger one
pixel_range_t get_centered(view_t const& large, view_t const& small)
{
	auto center_x = large.width() / 2;
	auto center_y = large.height() / 2;

	pixel_range_t range;
	range.x_begin = center_x - small.width() / 2;
	range.x_end = range.x_begin + small.width();
	range.y_begin = center_y - small.height() / 2;
	range.y_end = range.y_begin + small.height();

	return range;
}


// places the border and letter centered in the new image
void assemble_blocks(
	view_t const& border_v,
	view_t const& letter_v,
	std::vector<view_t> const& dst_list,
	color const& letter_c,
	color const& surface_c)
{

	// determine where to place the letter
	const auto letter_range = get_centered(dst_list[0], letter_v);

	// predicate for determining if an x,y position should have the letter color
	const auto is_letter = [&](auto x, auto y)
	{
		// is is inside the dimenstions of the view?
		if (x < letter_range.x_begin || x >= letter_range.x_end || y < letter_range.y_begin || y >= letter_range.y_end)
			return false;

		auto x_pos = x - letter_range.x_begin;
		auto y_pos = y - letter_range.y_begin;

		// is the pixel black?
		return to_bits32(*(letter_v.xy_at(x_pos, y_pos))) == BLACK32;
	};

	// determine where to place the border
	const auto border_range = get_centered(dst_list[0], border_v);

	// predicate for determining if an x,y position should have the border color  (black)
	const auto is_border = [&](auto x, auto y)
	{
		if (x < border_range.x_begin || x >= border_range.x_end || y < border_range.y_begin || y >= border_range.y_end)
			return false;

		auto x_pos = x - border_range.x_begin;
		auto y_pos = y - border_range.y_begin;

		return to_bits32(*(border_v.xy_at(x_pos, y_pos))) == BLACK32;
	};

	//predicate for determinining if an x,y position should have the surface color
	const auto is_surface = [&](auto x, auto y)
	{
		if (x < border_range.x_begin || x >= border_range.x_end || y < border_range.y_begin || y >= border_range.y_end)
			return false;

		auto x_pos = x - border_range.x_begin;
		auto y_pos = y - border_range.y_begin;

		return to_bits32(*(border_v.xy_at(x_pos, y_pos))) != BLACK32;
	};

	// iterate over every pixel and set its color
	for (index_t y = 0; y < dst_list[0].height(); ++y)
	{
		pixel_ptr_list_t dst_it_list;
		dst_it_list.reserve(dst_list.size());

		for(size_t i = 0; i < dst_list.size(); ++i)
			dst_it_list.push_back(dst_list[i].row_begin(y));

		for (index_t x = 0; x < dst_list[0].width(); ++x)
		{
			auto c = GREY;
			if (is_letter(x, y))
				c = letter_c;
			else if (is_surface(x, y))
				c = surface_c;
			else if (is_border(x, y))
				c = BLACK;

			for(auto& dst_it : dst_it_list)
				dst_it[x] = to_pixel(c);			
		}
	}
}


// draw a line in an image
void draw_line(view_t const& view, pixel_range_t const& range)
{
	for (index_t y = range.y_begin; y < range.y_end; ++y)
	{
		auto it = view.row_begin(y);
		for (index_t x = range.x_begin; x < range.x_end; ++x)
		{
			it[x] = to_pixel(BLACK);
		}
	}
}


// add randomly generated defects as horizontal lines
void add_defects(view_t const& view)
{
	unsigned min_lines = 3;
	unsigned max_lines = 10;
	unsigned line_thickness = 10;

	typedef struct
	{
		index_t y_pos;
		index_t begin;
		index_t end;
	}  x_range;

	// get number of defects
	std::random_device r;	
	std::default_random_engine eng(r());
	std::uniform_int_distribution<unsigned> uni_lines(min_lines, max_lines);
	auto num_lines = uni_lines(eng);

	std::uniform_int_distribution<index_t> y_dist(100, view.height() - 100);
	std::uniform_int_distribution<index_t> x_dist(100, view.width() - 200);
	std::uniform_int_distribution<index_t> len_dist(10, 100);	

	for (size_t i = 0; i < num_lines; ++i)
	{
		pixel_range_t r;
		r.y_begin = y_dist(eng);
		r.y_end = r.y_begin + line_thickness;
		r.x_begin = x_dist(eng);
		r.x_end = r.x_begin + len_dist(eng);

		draw_line(view, r);
	}
}


// builds all of the images 
void build_images(const char* alpha_path, const char* border_path, const char* pass_dir, const char* fail_dir)
{
	auto alpha_img = read_image_from_file(alpha_path);
	auto alpha_v = gil::view(alpha_img);

	auto letter_ranges = get_letter_ranges(alpha_v);

	auto border_img = read_image_from_file(border_path);
	auto border_v = gil::view(border_img);

	// decide the image dimensions
	auto const dst_w = border_v.width() + 96;
	auto const dst_h = border_v.height() + 96;

	auto const num_images = letter_ranges.size() * SURFACE_COLRS.size() * LETTER_COLORS.size();
	auto const idx_len = std::to_string(num_images).length();
	unsigned idx = 1;
	char idx_str[100];
	size_t pass_idx = 0;
	size_t fail_idx = 1;

	auto const sub_view = [&](auto& view, auto const& range)
	{
		auto width = range.x_end - range.x_begin;
		auto height = range.y_end - range.y_begin;
		return view_t(width, height, alpha_v.xy_at(range.x_begin, range.y_begin)); 
	};	

	uint8_t letter_index = 0; // for naming files after their letter

	for (auto const& range : letter_ranges)
	{
		auto const letter_v = sub_view(alpha_v, range);
		char block_letter = 'A' + letter_index;

		for (auto const& sc : SURFACE_COLRS)
		{
			for (auto const& lc : LETTER_COLORS)
			{
				sprintf_s(idx_str, "%0*d", (int)idx_len, idx++); // zero pad index number
				const auto file_name = std::string(idx_str) + "_" + block_letter + ".png";
				const auto pass_path = str_append_sub(pass_dir, file_name);
				const auto fail_path = str_append_sub(fail_dir, file_name);
				
				image_t pass_img(dst_w, dst_h);
				image_t fail_img(dst_w, dst_h);

				std::vector<view_t> view_list { gil::view(pass_img), gil::view(fail_img) };

				assemble_blocks(border_v, letter_v, view_list, lc, sc);

				// modify fail images
				add_defects(view_list[fail_idx]);

				gil::write_view(pass_path, view_list[pass_idx], gil::png_tag());
				gil::write_view(fail_path, view_list[fail_idx], gil::png_tag());
			}
		}

		++letter_index;
	}	
}
