#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <string>
#include <fstream>
#include <iostream>

// Holds information about the RGB values of the image
// Greyscale images still use this class, but all of the colours are set to the same value
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
    bool IsBlack() { return red == 0 && green == 0 && blue == 0; }
};

// Currently not used, may require a templateing of the image class
struct YCbCr 
{
    int y;
    int cr;
    int cb;
    YCbCr(){}
    YCbCr(int y_, int cb_, int cr_) 
    {
        y = y_;
        cb = cb_;
        cr = cr_;
    }

    YCbCr& operator=(YCbCr& other)
    {
        y = other.y;
        cb = other.cb;
        cr = other.cr;
        return *this;
    }
}; 

// An enum for setting the specific type of image, should be able to handle input of all different types
enum ImageType: int
{
    None,
    PBM,
    PGM,
    PPM
};

class Image
{
    private:
        // Data
        size_t m_width;
        size_t m_height;
        size_t m_colourDepth;
        size_t m_ID;
        ImageType m_type;
        RGB** m_pixels; // [height][width]

        static size_t m_IDGen;

        // Methods
        void ResizeImage(int width, int height, int colorDepth);
        void ClearImageData();

    public:
        // Constructors
        Image();
        Image(std::string fileName);
        Image(Image& other);
        ~Image();

        // Methods
        RGB GetPixelValue(int row, int col);
        size_t GetWidth() { return m_width; }
        size_t GetHeight() { return m_height; }
        void SetPixelValue(int row, int col, RGB data);
        void ReadImage(std::string fileName);
        void WriteImage(std::string fileName);
        void PrintInfo();
        void NormalizeColour();
};

#endif //IMAGE_HPP_