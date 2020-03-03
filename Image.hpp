#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <string>
#include <fstream>
#include <iostream>


class Image
{
    private:
        size_t m_width;
        size_t m_height;
        size_t m_colorDepth;

        // for black and white images, only the red array is used
        int** m_red;
        int** m_green;
        int** m_blue;

        int m_imageType;

        static int m_ID;

        void ReadImageHeader(std::string fileName);
        void ResizeImage(int width, int height, int colorDepth);
        void ReadImage(std::string fileName);

    public:
        Image();
        Image(std::string fileName);
        Image(Image& other);
        ~Image();

        // return the value of the pixel at the spcified row, col, and colour array. For black and white images, the colour array is 0
        int GetPixelValue(int row, int col, int color = 0);
};

#endif //IMAGE_HPP_