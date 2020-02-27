#ifndef DISTRIBUTION_H_
#define DISTRIBUTION_H_

#include <chrono> 
#include <exception>
#include <fstream>  
#include <iostream> 
#include <math.h>   
#include <random>   
#include <string>   
#include <vector>   
#include <armadillo>

using namespace arma;

class Distribution
{
    //friend class Classifier;
    private:
        const std::string INPUT_DIRECTORY = "Input/";
        const std::string OUTPUT_DIRECTORY = "Output/";
        // Data Members
        const size_t m_id;
        std::string m_name;
        static size_t s_idGen;

        // Methods
        void ImportData(std::string inputFileName, int dimensions);
        double RandomNumberHelper();
        double BoxMuller(double m, double s);

    public:
        const size_t m_dimensions;
        Mat<double> m_meanMatrix; // a one dimensional array for storing the mean in x and y
        Mat<double> m_covarianceMatrix; //a two dimensional array for storing the covariance matrix
        std::vector<std::vector<double>> m_data;
        
        // Constructors / Destructors
        Distribution(size_t dimensions, Mat<double>, Mat<double>, std::string name = NULL);
        Distribution(int dimensions, std::string inputFileName, std::string name = "");
        ~Distribution();

        // Methods
        void PrintAll();
        void GenerateSamples(size_t num, std::string outputFileName);
        size_t GetID();
        std::string GetName();
        std::string GetInfo();
};
    
#endif //DISTRIBUTION_H_
