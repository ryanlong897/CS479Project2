#include <iostream>
#include <fstream>
#include <thread>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include "Image.hpp"


void GetMaskedImagePixelData(Image& mask, Image& image, Distribution& dist);
void CountMisclassifications(Image &mask, Image& image, std::string outputTextFile);
void Mask(Image& mask, Image& other);

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
        Distribution dist0(2, "modified dist 1");
        Distribution dist1(2, "modified dist 2");

        dist0.ImportData(INPUT_DIRECTORY + "part2_dist0.txt");
        dist1.ImportData(INPUT_DIRECTORY + "part2_dist1.txt");

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
        std::string outputPath1 = "Output/missclassifications_image6.txt";
        std::string outputPath2 = "Output/missclassifications_image3.txt";
        std::string outputPath3 = "Output/missclassifications_image6ycbcr.txt";
        std::string outputPath4 = "Output/missclassifications_image3ycbcr.txt";
        std::remove (outputPath1.c_str());
        std::remove (outputPath2.c_str());
        std::remove (outputPath3.c_str());
        std::remove (outputPath4.c_str());

        Image mask, trainingImage, testingImage1, testingImage2, originalImage3, originalImage6, trainingYCBCR;
        mask.ReadImage("ref1.ppm");
        
        trainingImage.ReadImage("Training_1.ppm");
        trainingYCBCR.ReadImage("Training_1.ppm");
        trainingYCBCR.ToYCbCr();
        testingImage1.ReadImage("Training_6.ppm");
        testingImage2.ReadImage("Training_3.ppm");
        trainingImage.NormalizeColour();
        testingImage1.NormalizeColour();
        testingImage2.NormalizeColour();

        originalImage6.ReadImage("Training_6.ppm");
        originalImage3.ReadImage("Training_3.ppm");


        Distribution skin(3, "skinColour");
        Distribution skinYCBCR(3, "YCBCR");
        GetMaskedImagePixelData(mask, trainingImage, skin);
        skin.GetMatricesFromData();
        GetMaskedImagePixelData(mask, trainingYCBCR, skinYCBCR);
        skinYCBCR.GetMatricesFromData();

        std::vector<Distribution> classes;
        std::vector<Distribution> classesYCBCR;
        classes.push_back(skin);
        classesYCBCR.push_back(skinYCBCR);

        skin.PrintAll();
        skinYCBCR.PrintAll();

        Image testingMask1("ref6.ppm");
        Image testingMask2("ref3.ppm");
        Classifier imageClassifier(classes);
        Classifier imageClassifierYCBCR(classesYCBCR);

        std::cout << "classifying image 6 RGB" << std::endl;
        for (double i = .02; i < .08; i += .001)
        {
            Image newImage(testingImage1);
            imageClassifier.ClassifyImage(newImage, "Test1.ppm", i);
            CountMisclassifications(testingMask1, newImage, outputPath1);

            
        }

        std::cout << "classifying image 3 RGB" << std::endl;
        for (double i = .02; i < .08; i += .001)
        {
            Image newImage(testingImage2);
            imageClassifier.ClassifyImage(newImage, "Test2.ppm", i);
            CountMisclassifications(testingMask2, newImage, outputPath2);
        }

        std::cout << "Classifying image 3 YCBCR" << std::endl;
        for (int i = 0; i < 255; i++)
        {
            Image newImage2(testingImage1);
            newImage2.ToYCbCr();
            imageClassifierYCBCR.ClassifyImage(newImage2, "Test1.ppm", i);
            CountMisclassifications(testingMask1, newImage2, outputPath3);
        }

        std::cout << "Classifying image 6 YCBCR" << std::endl;
        for (int i = 0; i < 255; i++)
        {
            Image newImage2(testingImage2);
            newImage2.ToYCbCr();
            imageClassifierYCBCR.ClassifyImage(newImage2, "Test2.ppm", i);
            CountMisclassifications(testingMask2, newImage2, outputPath4);
        }

        Image newImage2(testingImage2);
        imageClassifier.ClassifyImage(newImage2, "whatever", .045);
        Mask(newImage2, originalImage3);
        originalImage3.WriteImage("MaskedImage3.ppm");

        Image newImage(testingImage1);
        imageClassifier.ClassifyImage(newImage, "whatever", .045);
        Mask(newImage, originalImage6);
        originalImage6.WriteImage("MaskedImage6.ppm");

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
            else{
                RGB imagePixel = image.GetPixelValue(i, j);
                std::vector<double> values;
                values.push_back((double)imagePixel.red);
                values.push_back((double)imagePixel.blue);
                values.push_back((double)imagePixel.green);

                dist.AddData(std::vector<double>(values));
            }
        }
    }
}

void CountMisclassifications(Image &mask, Image& image, std::string outputTextFile)
{
    int falsePositive = 0, falseNegative = 0;
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
            RGB imagePixel = image.GetPixelValue(i, j);
            if (maskPixel.IsBlack() && !imagePixel.IsWhite())
            {
                falsePositive++;
            }
            else if(!maskPixel.IsBlack() && imagePixel.IsWhite()){
                falseNegative++;
            }
        }
    }

    std::ofstream output;
    output.open(outputTextFile, std::fstream::app);
    output << falseNegative << "\t" << falsePositive << std::endl;
    output.close();
}

void Mask(Image& mask, Image& other)
{
    if (mask.GetHeight() != other.GetHeight() || mask.GetWidth() != other.GetWidth())
    {
        std::cerr << "Error, mask and image are not the same size" << std::endl;
        exit(1);
    }

    for (size_t i = 0; i < mask.GetHeight(); i++)
    {
        for (size_t j = 0; j < mask.GetWidth(); j++)
        {
            RGB maskPixel = mask.GetPixelValue(i, j);
            if (maskPixel.IsWhite())
            {
                other.SetPixelValue(i, j, RGB(255, 255, 255));
            }
        }
    }
}