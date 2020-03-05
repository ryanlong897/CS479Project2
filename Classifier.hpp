#ifndef CLASSIFIER_H_
#define CLASSIFIER_H_

#include "Distribution.hpp"
#include "Image.hpp"
#include <vector>

class Classifier
{
    private:
        std::vector<double> m_priors;
        std::vector<Distribution> m_classes;
        std::vector<size_t> m_misclassified = {0, 0};

        void LinearDiscriminant(std::vector<mat>& w, std::vector<double>& w0);
        void QuadraticDiscriminant(std::vector<mat>& W, std::vector<mat>& w, std::vector<double>& w0);
    public:
        void CalculateDecisionBoundary();
        Classifier(std::vector<Distribution> classes, std::vector<double> priors = std::vector<double>() );
        ~Classifier();
        void ClassifyTwoClasses(std::string outputFile, int classificationMethod = 0);
        void ClassifyImage(Image& image, std::string outputImageName, double threshold);
        double CalculateBhattacharyyaBound();
};

#endif
