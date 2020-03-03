#include <iostream>
#include <fstream>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include "Image.hpp"
#include <thread>

int main()
{   
    int m, n, q;
    bool type;
    ImageParser::ReadImageHeader("Input/Training_3.ppm", n, m, q, type);
    Image image(n, m, q);
    ImageParser::ReadImage("Input/Training_3.ppm", image);
    ImageParser::WriteImage("Output/test.ppm", image);

    return 0;
}
