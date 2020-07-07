# Boost GIL
Examples using the Boost GIL library for working with images

## 01_Test
* Files from tutorials in the documentation

## BlockFactory
* C++17
* _CRT_SECURE_NO_WARNINGS (required for GIL to compile)
* Generates images of letter blocks
* Used to generate "pass" and "fail" images for a Machine Learning project
* Parses an image called alphabet.png and copies each letter from it
* Places each letter in the center of border.png
* Adds a grey background on the outside of the border
* Adds one of six different block surface colors within the border
* Adds one of six different letter colors
* Creates another folder of "fail" images with randomly added black lines as "defects"