#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include"../hpp/win32_leak_check.h"
#include"../hpp/image_scan.hpp"

int main() {
#if defined(_DEBUG)
	int dbgFlags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	dbgFlags |= _CRTDBG_CHECK_ALWAYS_DF;   // check block integrity
	dbgFlags |= _CRTDBG_DELAY_FREE_MEM_DF; // don't recycle memory
	dbgFlags |= _CRTDBG_LEAK_CHECK_DF;     // leak report on exit
	_CrtSetDbgFlag(dbgFlags);
#endif

	std::cout << "Reading image... ";
	auto img_file_path = "./media/colors-100x100.jpg";
	//auto img = read_image_jpg(img_file_path);

	auto img = read_image_jpg("./media/colors_try.png");

	std::cout << "Done\n";

	/*std::cout << "Scanning Image... ";
	fill_histogram(img);
	std::cout << "Done\n";

	std::cout << "Writing file... ";
	auto out_file = "./out_files/multi_hist.txt";
	write_histogram(out_file);
	std::cout << "Done\n";*/

}