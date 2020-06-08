#include"block_factory.hpp"



int main()
{
	auto alpha_path = "./src_media/alphabet.png";
	auto border_path = "./src_media/border.png";
	auto out_dir = "./dst_media/alphabet";

	build_images(alpha_path, border_path, out_dir);

	
	
}