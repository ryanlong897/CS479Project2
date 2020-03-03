#ifndef IMAGE_H
#define IMAGE_H

#include <string>

class Image {
    private:
        const int NUM_COLOURS = 3; // for RGB
        int N, M, Q;
        bool type;
        int colourSpace = 0; // 0 for RGB, 1 for the other one  
        int **pixelValue[3];

    public:
        Image();
        Image(int, int, int);
        Image(Image&);
        Image operator= (Image& other);
        ~Image();
        void getImageInfo(int&, int&, int&);
        void setImageInfo(int, int, int);
        void setPixelVal(int c, int, int, int);
        void getPixelVal(int c, int, int, int&);
};

class ImageParser
{
    private:

    public: 
        static int WriteImage(std::string fname, Image& image);
        static int ReadImage(std::string fname, Image& image);
        static int ReadImageHeader(std::string fname, int& N, int& M, int& Q, bool& type);
};

#endif