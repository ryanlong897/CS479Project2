#ifndef IMAGE_CPP_
#define IMAGE_CPP_

#include "Image.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

size_t Image::m_IDGen = 0;

/**
 * Default Constructor 
 * @brief initialises all variables to default values
 */ 
Image::Image()
    :m_width(0),
     m_height(0),
     m_colourDepth(0),
     m_ID(m_IDGen++),
     m_type(None),
     m_pixels(nullptr)
{
}

/**
 * "Import" constructor
 * @param fileName the name of the image file to import
 * @brief reads the image file and populates the class variables right from the image file
 */ 
Image::Image(std::string fileName)
    :m_width(0),
     m_height(0),
     m_colourDepth(0),
     m_ID(m_IDGen++),
     m_type(None),
     m_pixels(nullptr)
{
    ReadImage(fileName);
}

/**
 * Copy constructor 
 * @param other the image to be copied
 * @brief makes a deep copy of image other
 */ 
Image::Image(Image& other)
    :m_width(other.m_width),
     m_height(other.m_height),
     m_colourDepth(other.m_colourDepth),
     m_ID(m_IDGen++),
     m_type(other.m_type),
     m_pixels(new RGB*[other.m_height])
{
    for (size_t height = 0; height < m_height; height++)
    {
        m_pixels[height] = new RGB[m_width];
        for (size_t width = 0; width < m_width; width++)
        {
            SetPixelValue(height, width, other.GetPixelValue(height, width));
        }
    }
}

/**
 * Destructor
 */ 
Image::~Image()
{
    ClearImageData();
}

/**
 * Get Pixel Value
 * @brief gets the RGB value at the specified index
 * @param row the row to get the data from
 * @param col the column to get the data from
 * @return the RGB value at the given location
 */ 
RGB Image::GetPixelValue(int row, int col)
{
    return m_pixels[row][col];
}

/**
 * Set Pixel Value 
 * @param row the row
 * @param col the column
 * @param data the RGB value to set the pixel to
 */ 
void Image::SetPixelValue(int row, int col, RGB data)
{
    m_pixels[row][col] = data;
}

/**
 * Resize image
 * @param width the new width
 * @param height the new height
 * @param colourDepth the new colour depth
 * @brief clears the image contents (if there is something there) and resizes the image array to the specified size
 */ 
void Image::ResizeImage(int width, int height, int colorDepth)
{
    if (m_pixels != nullptr)
    {
        ClearImageData();
    }
    
    m_pixels = new RGB*[m_height];
    for (size_t i = 0; i < m_height; i++)
    {
        m_pixels[i] = new RGB[m_width];
    }
}

/**
 * Read image
 * @param fileName the image file to be read
 * @brief reads the image file specified 
 */ 
void Image::ReadImage(std::string fileName)
{
    if (m_pixels != nullptr)
    {
        ClearImageData();
    }

    unsigned char* buffer;

    std::ifstream input;
    input.open("Input/" + fileName, std::ios::in | std::ios::binary);

    if (!input.is_open())
    {
        std::cerr << "Error reading file " <<  fileName << std::endl;
        exit(1);
    }

    //get header
    std::string line;
    std::getline(input, line);

    if (line[0] != 'P' || line.length() < 2)
    {
        std::cerr << "File type not supported, or header format is wrong" << std::endl;
        exit(1);
    }
    
    //set image type depending on header
    switch (line[1])
    {
        case '1':
        case '4':
            m_type = PBM;
            break;
        case '2':
        case '5':
            m_type = PGM;
            break;
        case '3':
        case '6':
            m_type = PPM;
            break;
        default:
            std::cerr << "File type not recognized" << std::endl;
            exit(1);
    }

    // Read the rest of the header, taking into account that information could be in a variety of locations
    // e.g. on the same line or on different lines
    bool isHeaderComplete = false;
    do
    {
        std::string temp;
        std::vector<std::string> split;
        std::stringstream ss(line);
        while (std::getline(ss, temp, ' '))
        {
            if (temp[0] == 'P' || temp[0] == '#')
            {
                continue;
            }
            split.push_back(temp);
        }

        for (size_t i = 0; i < split.size(); i++)
        {
            if (m_width == 0)
            {
                m_width = std::atoi(split[i].c_str());
            }
            else if (m_height == 0)
            {
                m_height = std::atoi(split[i].c_str());
            }
            else if (m_colourDepth == 0)
            {
                m_colourDepth = std::atoi(split[i].c_str());
                isHeaderComplete = true;
                continue;
            }
        }
        if (!isHeaderComplete)
        {
            std::getline(input, line);
        }
    } while (!isHeaderComplete);

    //get the image properties ready to be read
    ResizeImage(m_width, m_height, m_colourDepth);

    size_t size = m_height * m_width * (m_type == PPM ? 3 : 1);
    buffer = (unsigned char*) new unsigned char[size];
    input.read(reinterpret_cast<char *>(buffer), size * sizeof(unsigned char));

    if (input.fail())
    {
        std::cerr << "Image " << fileName << " has wrong size" << std::endl;
        exit(1);
    }

    input.close();

    //Convert from raw format to integers
    if (m_type == PPM)
    {
        int r, g, b;
        for (size_t height = 0; height < m_height; height++)
        {
            for (size_t width = 0; width < m_width * 3; width += 3)
            {
                r = (int)buffer[height * m_width * 3 + width];
                g = (int)buffer[height * m_width * 3 + width + 1];
                b = (int)buffer[height * m_width * 3 + width + 2];

                SetPixelValue(height, width/3, RGB(r, g, b));
            }
        }
    }
    else 
    {
        int c;
        for (size_t height = 0; height < m_height; height++)
        {
            for (size_t width = 0; width < m_width; width++)
            {
                c = (int)buffer[height * m_width + width];

                SetPixelValue(height, width, RGB(c, c, c));
            }
        }
    }

    delete[] buffer;
}

/**
 * Write image
 * @param fileName the file to write image data to
 * @brief writes the contents of the image array to the specified file
 */ 
void Image::WriteImage(std::string fileName)
{
    std::ofstream output;
    output.open("Output/" + fileName, std::ios::out | std::ios::binary);

    if (!output.is_open())
    {
        std::cerr << "Error opening output file " << fileName << std::endl;
        exit(1);
    }

    //set the header appropriately
    switch (m_type)
    {
        case PPM:
            output << "P6" << std::endl;
            break;
        case PGM:
            output << "P5" << std::endl;
            break;
        case PBM:
            output << "P4" << std::endl;
        default:
            std::cerr << "error writing output data, file type not known" << std::endl;
            break;
    }

    output << m_width << " " << m_height << std::endl;
    output << m_colourDepth << std::endl;

    unsigned char* buffer;
    size_t size = m_height * m_width * (m_type == PPM ? 3 : 1);

    buffer = (unsigned char*) new unsigned char[size];

    //convert the integer values into raw data
    if (m_type == PPM)
    {
        for (size_t height = 0; height < m_height; height++)
        {
            for (size_t width = 0; width < (m_width * 3); width += 3)
            {
                RGB data = GetPixelValue(height, width/3);
                buffer[(height * m_width * 3) + width] = data.red < 1 ? (unsigned char)(data.red * m_colourDepth) : (unsigned char)data.red;
                buffer[(height * m_width * 3) + width + 1] = data.green < 1 ? (unsigned char)(data.green * m_colourDepth) : (unsigned char)data.green;
                buffer[(height * m_width * 3) + width + 2] = data.blue < 1 ? (unsigned char)(data.blue * m_colourDepth) : (unsigned char)data.blue;
            }
        }
    }
    else 
    {
        for (size_t height = 0; height < m_height; height++)
        {
            for (size_t width = 0; width < m_width; width++)
            {
                RGB data = GetPixelValue(height, width);
                buffer[height * m_width + width] = data.red;
            }
        }
    }

    //write it
    output.write(reinterpret_cast<char *>(buffer), size * sizeof(unsigned char));

    if (output.fail()) {
      std::cout << "Can't write image " << fileName << std::endl;
      exit(1);
    }

    output.close();

    delete[] buffer;

}

/**
 * Print Info 
 * @brief prints debug info about the image
 */ 
void Image::PrintInfo()
{
    std::cout << "Width:\t" << m_width << std::endl;
    std::cout << "Height:\t" << m_height << std::endl;
    std::cout << "Depth:\t" << m_colourDepth << std::endl;
    std::cout << "Type:\t" << m_type << std::endl;
}

/**
 * Clear image data
 * @brief deleted dynamic memory stored by the image
 */ 
void Image::ClearImageData()
{
    for (size_t i = 0; i < m_height; i++)
    {
        delete[] m_pixels[i];
    }
    delete[] m_pixels;
}

/**
 * Normalize colour
 * @brief changes the default RGB colours into normalized RGB colours 
 */ 
void Image::NormalizeColour()
{
    for (size_t i = 0; i < m_height; i++)
    {
        for (size_t j = 0; j < m_width; j++)
        {
            RGB pixel = GetPixelValue(i, j);
            int denominator = pixel.red + pixel.green + pixel.blue;
            double r = (denominator != 0) ? pixel.red / denominator : 0;
            double g = (denominator != 0) ? pixel.green / denominator : 0;
            double b = (denominator != 0) ? pixel.blue / denominator : 0;
            SetPixelValue(i, j, RGB(r, g, b));
        }
    }
}

void Image::ToYCbCr()
{
    for (size_t i = 0; i < m_height; i++)
    {
        for (size_t j = 0; j < m_width; j++)
        {
            RGB pixel = GetPixelValue(i, j);
            double r = (.299 * pixel.red + .587 * pixel.green + .144 * pixel.blue) / m_colourDepth;
            double g = (-.169 * pixel.red - .332 * pixel.green + .5 * pixel.blue) / m_colourDepth;
            double b = (.5 * pixel.red - .419 * pixel.green -.081 * pixel.blue) / m_colourDepth;
            SetPixelValue(i, j, RGB(r, g, b, true));

        }
    }
}


#endif //IMAGE_CPP_