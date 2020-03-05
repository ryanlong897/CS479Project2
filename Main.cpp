#include <iostream>
#include <fstream>
#include <thread>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include "Image.hpp"

void GetMaskedImagePixelData(Image& mask, Image& image, Distribution& dist);

int main(int argc, char* argv[])
{   
    /**
     * Part 2 stuff
     */ 
    std::vector<double> priors;
    priors.push_back(.5);
    priors.push_back(.5);

    Image mask, trainingImage;
    mask.ReadImage("ref3.ppm");
    trainingImage.ReadImage("Training_3.ppm");
    Distribution skin(3, "skinColour");
    Distribution test(3, "possibly skin colour");

    std::vector<Distribution> classes;
    classes.push_back(skin);
    classes.push_back(test);


    trainingImage.NormalizeColour();
    GetMaskedImagePixelData(mask, trainingImage, skin);

    skin.GetMatricesFromData();
    skin.PrintAll();
    test.PrintAll();

    // TODO: Implement the threshold classifier 
    // TODO: Modify the image or classifier class to handle thresholds rather than just comparing against another class

    /**
     * Part 1 stuff
     */ 

    // Distribution dist(2, "Output0");
    // dist.ImportData("Input/dist0_output.txt");
    // dist.GetMatricesFromData();
    // dist.PrintAll();
    return 0;
}

/**
 * Get masked image pixel data
 * @param mask the image that is to act as a mask (expects black and white)
 * @param image the image that is to be compared to the mask
 * @param dist the distribution to save pixel data into
 * @brief reads in the pixels from image that are not being masked by mask and saves the RGB values of the relevant pixels into dist's data
 */ 
void GetMaskedImagePixelData(Image& mask, Image& image, Distribution& dist)
{
    if (mask.GetHeight() != image.GetHeight() || mask.GetWidth() != image.GetWidth())
    {
        std::cerr << "Error, mask and image are not the same size" << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < mask.GetHeight(); i++)
    {
        for (size_t j = 0; j < mask.GetWidth(); j++)
        {
            RGB maskPixel = mask.GetPixelValue(i, j);
            if (maskPixel.IsBlack())
            {
                continue;
            }
            
            RGB imagePixel = image.GetPixelValue(i, j);
            std::vector<double> values;
            values.push_back((double)imagePixel.red);
            values.push_back((double)imagePixel.blue);
            values.push_back((double)imagePixel.green);

            dist.AddData(std::vector<double>(values));
        }
    }
}