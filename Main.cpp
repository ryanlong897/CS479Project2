#include <iostream>
#include <fstream>
#include "Distribution.hpp"
#include "Classifier.hpp"
#include <thread>

const size_t NUM_SAMPLES = 100000;

int main()
{   
    // Generate distributions
    Distribution dist1(2, "dist1.txt", "Original Dist1");
    Distribution dist2(2, "dist2.txt", "Original Dist2");
    Distribution dist3(2, "dist3.txt", "Modified Dist2");

    //Print distributions to ensure data is input correctly
    dist1.PrintAll();
    dist2.PrintAll();
    dist3.PrintAll();
    
    //Initialize equal priors
    std::vector<double> priors;
    priors.push_back(.5);
    priors.push_back(.5);
    
    //Initialize unequal priors
    std::vector<double> modifiedPriors;
    modifiedPriors.push_back(.2);
    modifiedPriors.push_back(.8);

    // Initialize class vector
    std::vector<Distribution> classes;
    classes.push_back(dist1);
    classes.push_back(dist2);

    /**
     * Part 1 & also 2 
     * populate classes with data
     */ 
    for (size_t i = 0; i < classes.size(); i++)
    {
        std::string fileName = "dist" + std::to_string(i) + "_output.txt";
        classes[i].GenerateSamples(NUM_SAMPLES, fileName);
    }

    //Classify the data
    Classifier classifier(priors, classes);
    Classifier modifiedClassifier(modifiedPriors, classes);
    classifier.ClassifyTwoClasses("Classification.txt");
    modifiedClassifier.ClassifyTwoClasses("ModifiedClassification.txt");

    // classifier.ClassifyTwoClasses("minDistance.txt", 1);
    // modifiedClassifier.ClassifyTwoClasses("modifiedMinDistance.txt", 3);

    //Compute bhattacharyya bound
    classifier.CalculateBhattacharyyaBound();
    modifiedClassifier.CalculateBhattacharyyaBound();

    return 0;
}
