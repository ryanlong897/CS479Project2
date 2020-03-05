#ifndef CLASSIFIER_CPP_
#define CLASSIFIER_CPP_

#include "Classifier.hpp"
#include "math.h"
using namespace arma;

void Classifier::CalculateDecisionBoundary()
{

}

/**
 * Constructor
 * @param priors a vector representing the prior probablility for the classes
 * @param classes a vector containing the classes as distributions
 */ 
Classifier::Classifier(std::vector<Distribution> classes, std::vector<double> priors)
{
    if (priors.empty())
    {
        for (size_t i = 0; i < classes.size(); i++)
        {
            priors.push_back( 1.0 / classes.size());
        }
    }
    if (priors.size() != classes.size())
    {
        throw std::logic_error("There must be an equal amount of prior probabilities as there are classes\n");
    }
    m_priors = std::vector<double>(priors);
    m_classes = std::vector<Distribution>(classes);
}

/**
 * Destructor
 */ 
Classifier::~Classifier()
{}

/**
 * Classify Two classes
 * @param outputFile the file to output results to
 * @param classificationMethod the method used to classify. 0(default) = auto, 1 = min dist, 2 = linear, 3 = quadratic
 * 
 * @brief uses bayes classifier to classify data into one of two classes
 */ 
void Classifier::ClassifyTwoClasses(std::string outputFile, int classificationMethod)
{
    //For right now, this entire class can only contain two elements though I may change this in the future
    if (m_classes.size() != 2)
    {
        throw std::logic_error("Improper number of classes for this function\n");
    }

    for (size_t i = 0; i < m_misclassified.size(); i++)
    {
        m_misclassified[i] = 0;
    }

    //Set up the output file
    std::cout << "Classifying " << m_classes[0].GetInfo() << " & " << m_classes[1].GetInfo() << " using priors: " << std::endl 
              << "P(w" << m_classes[0].GetID() << ") = " << m_priors[0] << ", P(w" << m_classes[1].GetID() << ") = " << m_priors[1] << std::endl;
    std::string fullPath = "Output/" + outputFile;
    std::remove (fullPath.c_str());
    std::ofstream output;
    output.open(fullPath);
    
    std::string boundaryPath = "Output/boundaryPoints_" + outputFile;
    std::ofstream boundaryPoints;
    boundaryPoints.open(boundaryPath);

    std::vector<Mat<double>> W;
    std::vector<Mat<double>> w;
    std::vector<double> w0;
    
    int count = 0;

    // if automatic classification descriminant has been chosen, determine which is more appropriate to use
    if (classificationMethod == 0)
    {
        if (approx_equal(m_classes[0].m_covarianceMatrix, m_classes[1].m_covarianceMatrix, "absdiff", 0.001))
        {
            bool diag = true;
            int identity = m_classes[0].m_covarianceMatrix.diag()(0);
            for (size_t i = 0; i < m_classes[0].m_dimensions; i++)
            {
                if (m_classes[0].m_covarianceMatrix.diag()(i) != identity)
                {
                    diag = false;
                }
            }
            if (diag)
            {
                classificationMethod = 1;
            }
            else
            {
                classificationMethod = 2;
            }
        }
        else 
        {  
            classificationMethod = 3;
        }
    }

    switch (classificationMethod)
    {
    case 1:
        std::cout << "Using a Minimum Distance Classifier..." << std::endl;
        break;
    case 2:
        std::cout << "Using a linear discriminant..." << std::endl; 
        LinearDiscriminant(w, w0);       
        break;
    case 3:
        std::cout << "Using a quadratic discriminant..." << std::endl;
        QuadraticDiscriminant(W, w, w0);
        break;
    default:
        throw std::logic_error("Unknown discriminant method. Choose a value between 0 and 3\n");
        break;
    }

    output << "x\ty\tclass\tactual" << std::endl;
    for (size_t i = 0; i < m_classes.size(); i++)
    {
        for (size_t j = 0; j < m_classes[i].m_data.size(); j++)
        {
            double result = 0;
            switch (classificationMethod)
            {
            case 1:
                //gi(x) = -||x-mu||^2
                result = (-pow(norm(mat(m_classes[i].m_data[j]) - m_classes[0].m_meanMatrix), 2) + log(m_priors[0])) 
                       - (-pow(norm(mat(m_classes[i].m_data[j]) - m_classes[1].m_meanMatrix), 2) + log(m_priors[1]));
                //result *= -1;
                break;
            case 2:
                // gi(x) = w^t * x + w0
                result = as_scalar( (w[0].t() * mat(m_classes[i].m_data[j])) + w0[0] ) 
                       - as_scalar( (w[1].t() * mat(m_classes[i].m_data[j])) + w0[1] );
                break;
            case 3:
                //gi(x) = (x^t * W * x) + (w^t * x) + w0 
                result = as_scalar( (mat (m_classes[i].m_data[j]).t() * W[0] * mat(m_classes[i].m_data[j])) + ( w[0].t() * mat(m_classes[i].m_data[j]) ) + w0[0]) 
                       - as_scalar( (mat (m_classes[i].m_data[j]).t() * W[1] * mat(m_classes[i].m_data[j])) + ( w[1].t() * mat(m_classes[i].m_data[j]) ) + w0[1]);
                break;
            default:
                result = 0;
                break;
            }

            // g(x) = g1(x) - g2(x)
            // if g(x) > 0, choose g1, otherwise choose g2
            size_t determinedClass = (result > 0 ? m_classes[0].GetID() : m_classes[1].GetID());
            output << m_classes[i].m_data[j][0] << "\t" << m_classes[i].m_data[j][1] << "\t" << determinedClass << "\t" << m_classes[i].GetID() << std::endl;
            if (determinedClass != m_classes[i].GetID())
            {
                m_misclassified[i]++;
            }
            
            if (result < .05 && result > -.05) {
                boundaryPoints << m_classes[i].m_data[j][0] << "\t" << m_classes[i].m_data[j][1] << endl;
                count++;
            }
        }
    }
    
    std::cout << "num points on bound: " << count << endl;

    int totalMissclassified = 0;
    for (size_t i = 0; i < m_classes.size(); i++)
    {
        std::cout << "num misclassified (" << m_classes[i].GetID() << " as " << m_classes[(i + 1) % m_classes.size()].GetID() << "): " << m_misclassified[i] << '\n';
        totalMissclassified += m_misclassified[i];
    } 
    std::cout << "Total Misclassified: " << totalMissclassified << std::endl;
    std::cout << std::endl;
    boundaryPoints.close();
}

/**
 * Linear Discriminant
 * @param w an empty vector that will be populated with the value of w for all classes that exist in m_classes
 * @param w0 an empty vector that will be populated with the value of w0 for all classes that exist in m_classes
 */ 
void Classifier::LinearDiscriminant(std::vector<mat>& w, std::vector<double>& w0)
{
    //Get Equations
    for (size_t i = 0; i < m_classes.size(); i++)
    {
        // w = s^-1*mu
        w.push_back(inv(m_classes[i].m_covarianceMatrix) * m_classes[i].m_meanMatrix);
        // w0 = -1/2 * mu^t * s^-1 * mu + ln(P(w)) 
        w0.push_back(as_scalar(-.5 * (m_classes[i].m_meanMatrix.t() * (inv(m_classes[i].m_covarianceMatrix)) * m_classes[i].m_meanMatrix)) + log(m_priors[i]));
    }
}

/**
 * Quadratic Discriminant
 * @param W an empty vector that will be populated with the value of W for all classes that exist in m_classes
 * @param w an empty vector that will be populated with the value of w for all classes that exist in m_classes
 * @param w0 an empty vector that will be populated with the value of w0 for all classes that exist in m_classes
 */ 
void Classifier::QuadraticDiscriminant(std::vector<mat>& W, std::vector<mat>& w, std::vector<double>& w0)
{
    //Get Equations
    for (size_t i = 0; i < m_classes.size(); i++)
    {
        // W = -1/2 * S^-1
        W.push_back(-.5 * (inv(m_classes[i].m_covarianceMatrix)));
        // w = S^-1*mu
        w.push_back(inv(m_classes[i].m_covarianceMatrix) * m_classes[i].m_meanMatrix);
        // w0 = -1/2 * mu^t * S^-1 * mu - 1/2 * ln(|S|) + ln(P(w)) 
        w0.push_back( as_scalar(-.5 * (m_classes[i].m_meanMatrix.t() * inv(m_classes[i].m_covarianceMatrix) * m_classes[i].m_meanMatrix)) - as_scalar(.5 * log(det(m_classes[i].m_covarianceMatrix))) + log(m_priors[i]));
    }
}

/**
 * Calculate Bhattacharyya bound
 * @brief calculates the bhattacharyya bound for the data stored in this class
 */ 
double Classifier::CalculateBhattacharyyaBound() {
    double beta = .5;
    double k, firstTerm, secondTerm, p_error;
    Mat<double> meanDiff, betaTimesCov1PlusCov2, firstTermMatrix;
    
    std::cout << "Calculating Bhattacharyya Bound:" << endl;
    
    meanDiff = m_classes[0].m_meanMatrix - m_classes[1].m_meanMatrix;
    betaTimesCov1PlusCov2 = (1 - beta) * m_classes[0].m_covarianceMatrix + beta * m_classes[1].m_covarianceMatrix;
     
    firstTermMatrix = (beta * beta / 2) * meanDiff.t() * betaTimesCov1PlusCov2.i() * meanDiff;
        
    firstTerm = firstTermMatrix(0);
        
    secondTerm = .5 * log ( det(betaTimesCov1PlusCov2) / (pow(det(m_classes[0].m_covarianceMatrix) * det(m_classes[1].m_covarianceMatrix), beta) ));
        
    k = firstTerm + secondTerm;
    
    std::cout << "k = " << k << endl;
    
    p_error = (pow((m_priors[0] * m_priors[1]), .5) * exp(-k));
    
    std::cout << "Bhattacharyya Error: " << "P(error) <= " << p_error << endl;
    
    
    return p_error;
}


#endif // CLASSIFIER_CPP_
