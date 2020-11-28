#include"block_factory.hpp"



int main()
{
	auto alpha_path = "./src_media/alphabet.png";
	auto border_path = "./src_media/border.png";
	auto pass_dir = "./out_pass";
	auto fail_dir = "./out_fail";

	build_images(alpha_path, border_path, pass_dir, fail_dir);	
}