#include"block_factory.hpp"



int main()
{
	auto alpha_path = "./src_media/alphabet.png";
	auto border_path = "./src_media/border.png";
	auto out_dir = "../dst_media/alphabet"; // create diretories alphabet/pass and alphabet/fail

	build_images(alpha_path, border_path, out_dir);	
}