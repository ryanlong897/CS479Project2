#include <iostream>
#include <fstream>
#include <thread>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include "Image.hpp"

//If you get compilation errors when multithreading stuff is trying to happen, set this to 0
#define multiThread 1

void GetMaskedImagePixelData(Image& mask, Image& image, Distribution& dist);
void CountMisclassifications(Image &mask, Image& image, std::string outputTextFile);
void ROCCurve(Image& image, Classifier& classifier, Image& mask, std::string outputPath, bool decimalThresholds);
void Mask(Image& mask, Image& other);

/**
 * Main
 * @brief performs the tasks required for project 2
 */ 
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
        std::vector<Distribution> part1Distributions;
        std::vector<Distribution> part2Distributions;

        Distribution part1a_dist1(2, "Part1, Dist 1");
        Distribution part1a_dist2(2, "Part1, Dist 2");
        Distribution part2a_dist1(2, "Part2, Dist 1");
        Distribution part2a_dist2(2, "Part2, Dist 2");

        part1a_dist1.ImportData(INPUT_DIRECTORY + "part1_dist0.txt");
        part1a_dist2.ImportData(INPUT_DIRECTORY + "part1_dist1.txt");
        part2a_dist1.ImportData(INPUT_DIRECTORY + "part2_dist0.txt");
        part2a_dist2.ImportData(INPUT_DIRECTORY + "part2_dist1.txt");
        
        part1a_dist1.GetMatricesFromData();
        part1a_dist2.GetMatricesFromData();
        part2a_dist1.GetMatricesFromData();
        part2a_dist2.GetMatricesFromData();

        int size = part1a_dist1.m_data.size();

        do
        {
            part1a_dist1.PrintAll();
            part1a_dist2.PrintAll();
            part2a_dist1.PrintAll();
            part2a_dist2.PrintAll();

            part1Distributions.clear();
            part2Distributions.clear();

            part1Distributions.push_back(part1a_dist1);
            part1Distributions.push_back(part1a_dist2);
            part2Distributions.push_back(part2a_dist1);
            part2Distributions.push_back(part2a_dist2);

            Classifier part1Classifier(part1Distributions);
            Classifier part2Classifier(part2Distributions);

#if multiThread
            std::thread classify1(&Classifier::ClassifyTwoClasses, (Classifier*)(&part1Classifier), "Part1a.txt", 0);
            std::thread classify2(&Classifier::ClassifyTwoClasses, (Classifier*)(&part2Classifier), "Part2a.txt", 0);

            classify1.join();
            classify2.join();

            std::thread classify3(&Classifier::CalculateDecisionBoundary, &part1Classifier);
            std::thread classify4(&Classifier::CalculateDecisionBoundary, &part2Classifier);

            classify3.join();
            classify4.join();
            
            std::thread classify5(&Classifier::CalculateBhattacharyyaBound, &part1Classifier);
            std::thread classify6(&Classifier::CalculateBhattacharyyaBound, &part2Classifier);

            classify5.join();
            classify6.join();
#else
            part1Classifier.ClassifyTwoClasses("Part1a.txt");
            part2Classifier.ClassifyTwoClasses("Part2a.txt");
            part1Classifier.CalculateDecisionBoundary();
            part2Classifier.CalculateDecisionBoundary();
            part1Classifier.CalculateBhattacharyyaBound();
            part2Classifier.CalculateBhattacharyyaBound();
#endif
            size /= 10;
            part1a_dist1.SetDataSize(size);
            part1a_dist2.SetDataSize(size);
            part2a_dist1.SetDataSize(size);
            part2a_dist2.SetDataSize(size);

            std::cout << "-------------------------------------------------------\n" << std::endl;
            std::cout << "Current Size: " << size << std::endl;
        } while (size >= 10);

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

        Image mask, trainingImage, testingImage6, testingImage3, originalImage3, originalImage6, trainingYCBCR, testingImage3YCBCR, testingImage6YCBCR;
        mask.ReadImage("ref1.ppm");
        trainingImage.ReadImage("Training_1.ppm");
        trainingYCBCR.ReadImage("Training_1.ppm");
        testingImage3.ReadImage("Training_3.ppm");
        testingImage6.ReadImage("Training_6.ppm");
        testingImage6YCBCR.ReadImage("Training_6.ppm");
        testingImage3YCBCR.ReadImage("Training_3.ppm");
        originalImage3.ReadImage("Training_3.ppm");
        originalImage6.ReadImage("Training_6.ppm");
        
        trainingYCBCR.ToYCbCr();
        testingImage3YCBCR.ToYCbCr();
        testingImage6YCBCR.ToYCbCr();
        trainingImage.NormalizeColour();
        testingImage3.NormalizeColour();
        testingImage6.NormalizeColour();

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

        Image testingMask6("ref6.ppm");
        Image testingMask3("ref3.ppm");
        Classifier imageClassifier(classes);
        Classifier imageClassifierYCBCR(classesYCBCR);


#if multiThread
        std::cout << "classifying image 6 RGB" << std::endl;
        std::thread image6(ROCCurve, std::ref(testingImage6), std::ref(imageClassifier), std::ref(testingMask6), outputPath1, true);
        std::cout << "classifying image 3 RGB" << std::endl;
        std::thread image3(ROCCurve, std::ref(testingImage3), std::ref(imageClassifier), std::ref(testingMask3), outputPath2, true);
        std::cout << "classifying image 6 YCBCR" << std::endl;
        std::thread image6ycbcr(ROCCurve, std::ref(testingImage6YCBCR), std::ref(imageClassifierYCBCR), std::ref(testingMask6), outputPath3, false);
        std::cout << "classifying image 3 YCBCR" << std::endl;
        std::thread image3ycbcr(ROCCurve, std::ref(testingImage3YCBCR), std::ref(imageClassifierYCBCR), std::ref(testingMask3), outputPath4, false);

        image6.join();
        image3.join();
        image6ycbcr.join();
        image3ycbcr.join();
#else
        for (double i = 0.0; i < .4; i += .001)
        {
            Image newImage(testingImage6);
            imageClassifier.ClassifyImage(newImage, "", i);
            CountMisclassifications(testingMask6, newImage, outputPath1);  
        }

        std::cout << "classifying image 3 RGB" << std::endl;
        for (double i = 0.0; i < .4; i += .001)
        {
            Image newImage(testingImage3);
            imageClassifier.ClassifyImage(newImage, "", i);
            CountMisclassifications(testingMask3, newImage, outputPath2);
        }

        std::cout << "Classifying image 6 YCBCR" << std::endl;
        for (double i = 0; i < 100; i += 1)
        {
            Image newImage2(testingImage6YCBCR);
            imageClassifierYCBCR.ClassifyImage(newImage2, "", i);
            CountMisclassifications(testingMask6, newImage2, outputPath3);
        }

        std::cout << "Classifying image 3 YCBCR" << std::endl;
        for (double i = 0; i < 100; i+= 1)
        {
            Image newImage2(testingImage3YCBCR);
            imageClassifierYCBCR.ClassifyImage(newImage2, "", i);
            CountMisclassifications(testingMask3, newImage2, outputPath4);
        }
#endif

        Image newImage(testingImage6);
        imageClassifier.ClassifyImage(newImage, "whatever", .045);
        Mask(newImage, originalImage6);
        originalImage6.WriteImage("MaskedImage6.ppm");

        Image newImage2(testingImage3);
        imageClassifier.ClassifyImage(newImage2, "whatever", .045);
        Mask(newImage2, originalImage3);
        originalImage3.WriteImage("MaskedImage3.ppm");
    }

    return 0;
}

void ROCCurve(Image& image, Classifier& classifier, Image& mask, std::string outputPath, bool decimalThresholds)
{
    double delta, max;
    if (decimalThresholds)
    {
        delta = .001;
        max = .4;
    }
    else
    {
        delta = .2;
        max = 50;
    }
    for (double i = 0; i < max; i += delta)
    {
        Image newImage(image);
        classifier.ClassifyImage(newImage, "", i);
        CountMisclassifications(mask, newImage, outputPath);
    }
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
                values.push_back((double)imagePixel.green);
                values.push_back((double)imagePixel.blue);

                dist.AddData(std::vector<double>(values));
            }
        }
    }
}

/**
 * 
 */ 
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
            else if(!maskPixel.IsBlack() && imagePixel.IsWhite())
            {
                falseNegative++;
            }
        }
    }

    std::ofstream output;
    output.open(outputTextFile, std::fstream::app);
    output << falseNegative << "\t" << falsePositive << std::endl;
    output.close();
}

/**
 * 
 */ 
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
                other.SetPixelValue(i, j, RGB(255, 255, 255, false));
            }
        }
    }
}