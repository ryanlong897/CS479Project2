#ifndef DISTRIBUTION_CPP_
#define DISTRIBUTION_CPP_

#include "Distribution.hpp"
#include <sstream>

size_t Distribution::s_idGen = 0;

/**
 * Distribution Constructor
 * @param dimensions the dimensionality of the distribution to be constructed (i.e. how many variables)
 * @param meanMatrix a Matrix object of size "dimensions" representing the means of each dimension of the data
 * @param covarianceMatrix a Matrix object of size dimensions x dimensions
 * @param name the name of the distribution
 * 
 * @brief allocates memory for the various matrices and populates them if applicable
 */ 
Distribution::Distribution(size_t dimensions, Mat<double> meanMatrix, Mat<double> covarianceMatrix, std::string name)
	:m_id(s_idGen++),
	m_name(name == "" ? std::to_string(m_id) : name),
	m_dimensions(dimensions),
	m_meanMatrix(meanMatrix),
	m_covarianceMatrix(covarianceMatrix)
{}

/**
 * Constructor but for when data is in a file
 * @param dimensions the dimesnionality of the distribution (i.e. how many variables)
 * @param inputFileName the name or file path of the file to load data from
 * @param name the name of the distribution
 * 
 * @brief reads data from a specially formatted input file and populates the matrices accordingly
 */ 
Distribution::Distribution(int dimensions, std::string inputFileName, std::string name)
	:m_id(s_idGen++),
	m_name(name == "" ? std::to_string(m_id) : name),
	m_dimensions(dimensions),
	m_meanMatrix(dimensions, 1),
	m_covarianceMatrix(dimensions, dimensions)
{
	try
	{
		ImportMatrices(inputFileName, m_dimensions);
	}
	catch (const std::invalid_argument & e)
	{
		std::cerr << e.what();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		std::cerr << "This most likely means that the input file was not formatted correctly" << std::endl;
	}
}

/**
 * Constructor for when data will be added at a later time
 * @param dimensions the dimensionality of the distribution
 * @param name the name of the distribution
 * 
 * @brief initializes all values to default, expects that data will be added through the import data function later
 */ 
Distribution::Distribution(int dimensions, std::string name)
	:m_id(s_idGen++),
	m_name(name == "" ? std::to_string(m_id) : name),
	m_dimensions(dimensions),
	m_meanMatrix(dimensions, 1),
	m_covarianceMatrix(dimensions, dimensions)
{

}

/**
 * @brief destructor
 */ 
Distribution::~Distribution()
{
}

/**
 * Print all
 * @brief prints all information about the distribution
 * 	currently just the mean matrix and standard deviation matrix
 */ 
void Distribution::PrintAll()
{
    std::cout << "Data for Distribution " << GetInfo() << std::endl;
    std::cout << std::endl << "Mean Matrix:\n";
    m_meanMatrix.print();
    std::cout << "Covariance Matrix:\n";
    m_covarianceMatrix.print();
    std::cout << std::endl;
}

/**
 * BoxMuller
 * @param m the mean of the data to be generated
 * @param s the standard deviation of the data to be generated
 * @return a double that will be normally distributed within the constraints of the properties input
 * 
 * @brief calculates a value that will be normally distributed with mean m and standard deviation s
 */ 
double Distribution::BoxMuller(double m, double s)	/* normal random variate generator */
{	
	double x1, x2, w, y1;
	static double y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * RandomNumberHelper() - 1.0;
			x2 = 2.0 * RandomNumberHelper() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 && w != 0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

/**
 * random number helper
 * @brief a helper function that returns a random number using the random library's mersenne twister
 * @return a pseudorandom number
 */ 
double Distribution::RandomNumberHelper()
{
    std::mt19937 generator(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> dist(0.0, 1.0);
	return dist(generator);;
}

/**
 * Generate Samples
 * @param num the number of samples to generate
 * @param outputFileName the name or path of the file to output to
 * 
 * @brief generates data and outputs it to a file
 */ 
void Distribution::GenerateSamples(size_t num, std::string outputFileName)
{
	std::string fullPath = OUTPUT_DIRECTORY + outputFileName;
	std::remove(outputFileName.c_str());
	std::ofstream outputFile;
	outputFile.open(fullPath);
	outputFile << m_name << " " << "[id: " << m_id << "]" << std::endl;

	std::cout << "Generating Data for \"" << m_name << "\" [id: " << m_id << "]" << std::endl; 

	for (size_t i = 0; i < num; i++)
	{
		std::vector<double> temp;
		for (size_t j = 0; j < m_dimensions; j++)
		{
			double data = BoxMuller(m_meanMatrix(j), m_covarianceMatrix(j, j));
			outputFile << data << "\t";
			temp.push_back(data);
		}
		outputFile << std::endl;
		m_data.push_back(std::vector<double>(temp));
		temp.clear();
		if (i % (num /10) == 0)
		{
			std::cout << ((i * 100) / num) << "\%" << std::endl;
		} 
	}
	outputFile.close();
	std::cout << "Done! Output written to " << fullPath <<"\n" << std::endl;
}

/**
 * @brief imports data from input files to populate the arrays
 * @param inputFile the file to look for data in
 * @param dimensions the number of dimensions the data is supposed to have
 */ 
void Distribution::ImportMatrices(std::string inputFileName, int dimensions)
{
	std::string fullPath = INPUT_DIRECTORY + inputFileName;
	std::ifstream inputFile;
	inputFile.open(fullPath);

	if (!inputFile.is_open())
	{
		throw std::invalid_argument("Input file could not be opened");
	}

	std::vector<std::string> allLines;
	std::vector<double> meanData;
	std::vector<std::vector<double>> covarianceData;

	//Read in all lines from the file
	std::string line;
	while (std::getline(inputFile, line))
	{
		allLines.push_back(line);
	}

	//Parse the input
	for (size_t i = 0; i < allLines.size(); i++)
	{
		switch(allLines[i][0])
		{
			case 'M':
				i++;
				do 
				{
					double temp;
					temp = std::stod(allLines[i]);
					meanData.push_back(temp);
					i++;
				} while (allLines[i][0] != '*');
				break;

			case 'C':
			case 'S':
				i++;
				do
				{
					std::vector<double> intermediate;
					std::string temp;
					std::stringstream ss(allLines[i]);
					while (std::getline(ss, temp, '\t'))
					{
						intermediate.push_back(std::stod(temp));
					}
					i++;
					covarianceData.push_back(intermediate);
				} while (allLines[i][0] != '*');
				break;

			case '#':
			case '\n':
			default:
				continue;
		}
	}

	m_meanMatrix = mat(meanData);
	//Set the matrices
	for (size_t i = 0; i < m_dimensions; i++)
	{
		m_meanMatrix(i) = meanData[i];
		for (size_t j = 0; j < m_dimensions; j++)
		{
			m_covarianceMatrix(i, j) = covarianceData[i][j];
		}
	}

	inputFile.close();
}

/**
 * Get id
 * @return the id of the current distribution
 */ 
size_t Distribution::GetID()
{
	return m_id;
}

/**
 * Get name
 * @return the name of the distribution
 */ 
std::string Distribution::GetName()
{
	return m_name;
}

/**
 * Get Info
 * @brief prints all info about the distribution in a uniform format
 */ 
std::string Distribution::GetInfo()
{
	return (std::string)("\"" + m_name + "\" [id: " + std::to_string(m_id) + "]");
}

/**
 * Add data 
 * @param data a vector with dimensions m_dimensions
 * 
 * @brief adds data from vector data to the m_data vector in the class
 */ 
void Distribution::AddData(std::vector<double> data)
{
	if (data.size() == m_dimensions)
	{
		m_data.push_back(data);
	}
	else 
	{
		throw std::logic_error("Data must have the same dimensionality as the containing distribution\n");
	}
}

/**
 * Import Data
 * @param inputFilePath the full path to the data to be imported
 * 
 * @brief reads the specified file's data into the distribution's inner data storage. 
 * 		  Note: assumes that the first line of the file is a header line, and skips it
 */ 
void Distribution::ImportData(std::string inputFilePath)
{
	std::ifstream input;
	input.open(inputFilePath);

	if (!input.is_open())
	{
		std::cerr << "Error opening input file: " << inputFilePath << std::endl;
		exit(1);
	}

	// need to get rid of the top line of the file (its saved with a header)
	std::string trash;
	std::getline(input, trash);

	// read the data
	while (!input.eof())
	{
		std::vector<double> buffer;
		for (size_t i = 0; i < m_dimensions; i++)
		{
			std::string temp;
			input >> temp;
			if (temp == "" || temp == "\n" || temp == "\t")
			{
				continue;
			}			
			buffer.push_back(std::stod(temp));
		}
		if (buffer.size() == m_dimensions)
		{
			m_data.push_back(std::vector<double>(buffer));
		}
	}

	input.close();
}

/**
 * Get Mean
 * @param variable the index of the dimension variable to get the mean for
 * @brief calculates the mean of the variable (dimension) specified
 * @return the mean of the data
 */ 
double Distribution::GetMean(size_t variable)
{
	if (variable >= m_dimensions)
	{
		std::cerr << "variable index must be less than the total number of dimensions (its an index)" << std::endl;
		exit(1);
	}

	double mean = 0;
	for (size_t i = 0; i < m_data.size(); i++)
	{
		mean += m_data[i][variable];
	}
	return mean / m_data.size();
}

/**
 * Get Covariance
 * @param var1 the first variable index
 * @param var2 the second variable index
 * 
 * @brief calculates the covariance of the data with respect to variables 1 and 2
 * @return the covariance of the data
 */ 
double Distribution::GetCovariance(size_t var1, size_t var2)
{
	if (var1 >= m_dimensions || var2 >= m_dimensions)
	{
		std::cerr << "variable index must be less than the total number of dimensions (its an index)" << std::endl;
		exit(1);
	}

	double covariance = 0;

	for (size_t i = 0; i < m_data.size(); i++)
	{
		covariance += (m_data[i][var1] - m_meanMatrix(var1)) * (m_data[i][var2] - m_meanMatrix(var2));
	}

	return var1 == var2 ? std::sqrt(covariance / m_data.size()) : covariance / m_data.size();
}

/**
 * Get Matrices From Data
 * @brief Calculates the mean and covariance for the datasets present in the m_data storage
 */ 
void Distribution::GetMatricesFromData()
{
	for (size_t i = 0; i < m_dimensions; i++)
	{
		m_meanMatrix(i) = GetMean(i);
	}

	for (size_t i = 0; i < m_dimensions; i++)
	{
		for (size_t j = 0; j < m_dimensions; j++)
		{
			m_covarianceMatrix(i, j) = GetCovariance(i, j);
		}
	}
}

#endif //DISTRIBUTION_CPP_
