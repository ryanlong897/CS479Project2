#include <iostream>
#include <fstream>
#include <thread>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include "Image.hpp"


void GetMaskedImagePixelData(Image& mask, Image& image, Distribution& dist);

int main(int argc, char* argv[])
{   
    int part = 0;
    if (argc > 1)
    {
        part = atoi(argv[1]);
    }

    /**
     * Part 1 stuff
     */
    if (part == 1 || part == 0)
    {
        Distribution dist0(2, "Original dist 1");
        Distribution dist1(2, "Original dist 2");

        dist0.ImportData(INPUT_DIRECTORY + "dist0_output.txt");
        dist1.ImportData(INPUT_DIRECTORY + "dist1_output.txt");

        dist0.GetMatricesFromData();
        dist1.GetMatricesFromData();

        dist0.PrintAll();
        dist1.PrintAll();

        std::vector<Distribution> distributions;
        distributions.push_back(dist0);
        distributions.push_back(dist1);

        Classifier classifier(distributions);
        classifier.ClassifyTwoClasses("ClassificationResults.txt");
        classifier.CalculateDecisionBoundary();
        classifier.CalculateBhattacharyyaBound();
    }

    /**
     * Part 2 stuff
     */ 
    if (part == 2 || part == 0)
    {
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
    }

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