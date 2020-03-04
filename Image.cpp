#ifndef IMAGE_CPP_
#define IMAGE_CPP_

#include "Image.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

template <typename T>
size_t Image<T>::m_IDGen = 0;

template <typename T>
Image<T>::Image()
    :m_width(0),
     m_height(0),
     m_colorDepth(0),
     m_ID(m_IDGen++),
     m_type(0),
     m_pixels(nullptr)
{
}

template <typename T>
Image<T>::Image(std::string fileName)
    :m_width(0),
     m_height(0),
     m_colorDepth(0),
     m_ID(m_IDGen++),
     m_type(None),
     m_pixels(nullptr)
{
    ReadImage(fileName);
}

template <typename T>
Image<T>::Image(Image& other)
    :m_width(other.m_width),
     m_height(other.m_height),
     m_colorDepth(other.m_colorDepth),
     m_ID(m_IDGen++),
     m_type(other.m_type),
     m_pixels(new T*[other.m_height])
{
    for (size_t height = 0; height < m_height; height++)
    {
        m_pixels[height] = new T[m_width];
        for (size_t width = 0; width < m_width; width++)
        {
            SetPixelValue(height, width, GetPixelValue(height, width));
        }
    }
}

template <typename T>
Image<T>::~Image()
{
    ClearImageData();
}

// return the value of the pixel at the spcified row, col, and colour array. For black and white images, the colour array is 0
template <typename T>
T Image<T>::GetPixelValue(int row, int col)
{
    return m_pixels[row][col];
}

template <typename T>
void Image<T>::SetPixelValue(int row, int col, T data)
{
    m_pixels[row][col] = data;
}

template <typename T>
void Image<T>::ReadImageHeader(std::string fileName)
{

}

template <typename T>
void Image<T>::ResizeImage(int width, int height, int colorDepth)
{
    m_pixels = new T*[m_height];
    for (size_t i = 0; i < m_height; i++)
    {
        m_pixels[i] = new T[m_width];
    }
}

template <typename T>
void Image<T>::ReadImage(std::string fileName)
{
    unsigned char* buffer;

    std::ifstream input;
    input.open("Input/" + fileName, ios::in | ios::binary);

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
        std::cerr << "File type not supported" << std::endl;
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

    bool headerRead = false;
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
                headerRead = true;
                continue;
            }
        }
        if (!headerRead)
        {
            std::getline(input, line);
        }
    } while (!headerRead);

    ResizeImage(m_width, m_height, m_colorDepth);

    size_t size = m_height * m_width * (m_type == PPM ? 3 : 1);
    buffer = (unsigned char*) new unsigned char[size];
    input.read(reinterpret_cast<char *>(buffer), size * sizeof(unsigned char));


    if (input.fail())
    {
        std::cerr << "Image " << fileName << " has wrong size" << endl;
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
                r = (int)buffer[height * m_width + width];
                g = (int)buffer[height * m_width + width + 1];
                b = (int)buffer[height * m_width + width + 2];
                try
                {
                    SetPixelValue(height, width/3, T(r, g, b));
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Type does not contain a constructor that takes three arguments" << std::endl;
                    exit(1);
                }
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
                try
                {
                    SetPixelValue(height, width, T(c, c, c));
                }
                catch (const std::exception& e)
                {
                    std::cerr << "Type does not contain a constructor that takes three arguments" << std::endl;
                    exit(1);
                }
            }
        }
    }
    delete[] buffer;
}

template <typename T>
void Image<T>::PrintInfo()
{
    std::cout << "Width:\t" << m_width << std::endl;
    std::cout << "Height:\t" << m_height << std::endl;
    std::cout << "Depth:\t" << m_colorDepth << std::endl;
    std::cout << "Type:\t" << m_type << std::endl;
}

template <typename T>
void Image<T>::ClearImageData()
{
    for (size_t i = 0; i < m_height; i++)
    {
        delete[] m_pixels[i];
    }
    delete[] m_pixels;
}


#endif //IMAGE_CPP_