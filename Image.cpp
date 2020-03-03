#ifndef IMAGE_CPP_
#define IMAGE_CPP_

#include "Image.hpp"

Image::Image()
{

}

Image::Image(std::string fileName)
{

}

Image::Image(Image& other)
{

}

Image::~Image()
{

}

// return the value of the pixel at the spcified row, col, and colour array. For black and white images, the colour array is 0
int Image::GetPixelValue(int row, int col, int color = 0)
{

}

void Image::ReadImageHeader(std::string fileName)
{

}

void Image::ResizeImage(int width, int height, int colorDepth)
{

}

void Image::ReadImage(std::string fileName)
{

}


#endif //IMAGE_CPP_