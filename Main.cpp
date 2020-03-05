#include <iostream>
#include <fstream>
#include <thread>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include "Image.hpp"

int main(int argc, char* argv[])
{   
    Image image;
    if (argc > 1)
    {
        image.ReadImage(argv[1]);
    }
    else 
    {
        image.ReadImage("Training_1.ppm");
    }
    image.WriteImage("test1.ppm");
    return 0;
}
