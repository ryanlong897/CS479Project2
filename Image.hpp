#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <string>
#include <fstream>
#include <iostream>

struct RGB
{
    int red;
    int green;
    int blue;
    RGB(){}
    RGB(int r, int g, int b) 
    { 
        red = r; 
        green = g; 
        blue = b; 
    }

    RGB& operator= (RGB& other) 
    { 
        red = other.red; 
        green = other.green; 
        blue = other.blue;
        return *this;
    }
};

struct YCrCb
{
    int y;
    int cr;
    int cb;
    YCrCb(){}
    YCrCb(int y_, int cr_, int cb_) 
    {
        y = y_;
        cr = cr_;
        cb = cb_;
    }
    
    YCrCb& operator=(YCrCb& other)
    {
        y = other.y;
        cr = other.cr;
        cb = other.cb;
        return *this;
    }
}; 

enum ImageType: int
{
    None,
    PBM,
    PGM,
    PPM
};

template <typename T>
class Image
{
    private:
        size_t m_width;
        size_t m_height;
        size_t m_colorDepth;
        size_t m_ID;

        ImageType m_type;

        // [height][width]
        T** m_pixels;

        static size_t m_IDGen;

        void ReadImageHeader(std::string fileName);
        void ResizeImage(int width, int height, int colorDepth);
        void ReadImage(std::string fileName);
        void ClearImageData();

    public:
        Image();
        Image(std::string fileName);
        Image(Image& other);
        ~Image();

        // return the value of the pixel at the spcified row, col, and colour array. For black and white images, the colour array is 0
        T GetPixelValue(int row, int col);
        void SetPixelValue(int row, int col, T data);
        void WriteImage(std::string fileName);
        void PrintInfo();
};

#include "Image.cpp"

#endif //IMAGE_HPP_