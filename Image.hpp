#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <string>
#include <fstream>
#include <iostream>

// Holds information about the RGB values of the image
// Greyscale images still use this class, but all of the colours are set to the same value
struct RGB
{
    double red;
    double green;
    double blue;
    RGB(){}
    RGB(double r, double g, double b) 
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
    bool IsWhite() { return red == 255 && green == 255 && blue == 255; }
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
        void ToYCbCr();
};

#endif //IMAGE_HPP_