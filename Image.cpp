#ifndef IMAGE_CPP_
#define IMAGE_CPP_

#include "Image.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

size_t Image::m_IDGen = 0;

Image::Image()
    :m_width(0),
     m_height(0),
     m_colorDepth(0),
     m_ID(m_IDGen++),
     m_type(None),
     m_pixels(nullptr)
{
}

Image::Image(std::string fileName)
    :m_width(0),
     m_height(0),
     m_colorDepth(0),
     m_ID(m_IDGen++),
     m_type(None),
     m_pixels(nullptr)
{
    ReadImage(fileName);
}

Image::Image(Image& other)
    :m_width(other.m_width),
     m_height(other.m_height),
     m_colorDepth(other.m_colorDepth),
     m_ID(m_IDGen++),
     m_type(other.m_type),
     m_pixels(new RGB*[other.m_height])
{
    for (size_t height = 0; height < m_height; height++)
    {
        m_pixels[height] = new RGB[m_width];
        for (size_t width = 0; width < m_width; width++)
        {
            SetPixelValue(height, width, GetPixelValue(height, width));
        }
    }
}

Image::~Image()
{
    ClearImageData();
}

// return the value of the pixel at the spcified row, col, and colour array. For black and white images, the colour array is 0
RGB Image::GetPixelValue(int row, int col)
{
    return m_pixels[row][col];
}

void Image::SetPixelValue(int row, int col, RGB data)
{
    m_pixels[row][col] = data;
}

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
            else if (m_colorDepth == 0)
            {
                m_colorDepth = std::atoi(split[i].c_str());
                isHeaderComplete = true;
                continue;
            }
        }
        if (!isHeaderComplete)
        {
            std::getline(input, line);
        }
    } while (!isHeaderComplete);

    ResizeImage(m_width, m_height, m_colorDepth);

    size_t size = m_height * m_width * (m_type == PPM ? 3 : 1);
    buffer = (unsigned char*) new unsigned char[size];
    input.read(reinterpret_cast<char *>(buffer), size * sizeof(unsigned char));

    if (input.fail())
    {
        std::cerr << "Image " << fileName << " has wrong size" << std::endl;
        exit(1);
    }

    input.close();

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

void Image::WriteImage(std::string fileName)
{
    std::ofstream output;
    output.open("Output/" + fileName, std::ios::out | std::ios::binary);

    if (!output.is_open())
    {
        std::cerr << "Error opening output file " << fileName << std::endl;
        exit(1);
    }

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
    output << m_colorDepth << std::endl;

    unsigned char* buffer;
    size_t size = m_height * m_width * (m_type == PPM ? 3 : 1);
    std::cout << "going to write " << size << " bytes" << std::endl;
    buffer = (unsigned char*) new unsigned char[size];

    if (m_type == PPM)
    {
        for (size_t height = 0; height < m_height; height++)
        {
            for (size_t width = 0; width < (m_width * 3); width += 3)
            {
                RGB data = GetPixelValue(height, width/3);
                buffer[(height * m_width * 3) + width] = (unsigned char)data.red;
                buffer[(height * m_width * 3) + width + 1] = (unsigned char)data.green;
                buffer[(height * m_width * 3) + width + 2] = (unsigned char)data.blue;
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

    output.write(reinterpret_cast<char *>(buffer), size * sizeof(unsigned char));

    if (output.fail()) {
      std::cout << "Can't write image " << fileName << std::endl;
      exit(1);
    }

    output.close();

    delete[] buffer;

}

void Image::PrintInfo()
{
    std::cout << "Width:\t" << m_width << std::endl;
    std::cout << "Height:\t" << m_height << std::endl;
    std::cout << "Depth:\t" << m_colorDepth << std::endl;
    std::cout << "Type:\t" << m_type << std::endl;
}

void Image::ClearImageData()
{
    for (size_t i = 0; i < m_height; i++)
    {
        delete[] m_pixels[i];
    }
    delete[] m_pixels;
}


#endif //IMAGE_CPP_