
#include "Image.hpp"

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;

int ImageParser::ReadImage(std::string fname, Image& image)
{
    int i, j;
    int N, M, Q;
    unsigned char *charImage;
    char header [100], *ptr;
    ifstream ifp;

    ifp.open(fname, ios::in | ios::binary);

    if (!ifp) {
      cout << "Can't read image: " << fname << endl;
      exit(1);
    }

    // read header

    ifp.getline(header,100,'\n');
    
    if (header[0] != 'P')
    {
        std::cerr << "Type not recognized :/" << std::endl; 
        exit(1);
    }

    ifp.getline(header,100,'\n');
    while(header[0]=='#')
      ifp.getline(header,100,'\n');

    M=strtol(header,&ptr,0);
    N=atoi(ptr);

    ifp.getline(header,100,'\n');
    Q=strtol(header,&ptr,0);

    charImage = (unsigned char *) new unsigned char [M*N];

    ifp.read( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char));

    if (ifp.fail()) {
      cout << "Image " << fname << " has wrong size" << endl;
      exit(1);
    }

    ifp.close();

    //
    // Convert the unsigned characters to integers
    //

    int val;
    // FIX THIS SO IT CAN TAKE IN COLOURED IMAGES
    for(i=0; i<N; i++)
      for(j=0; j<M; j++) {
        val = (int)charImage[i*M+j];
        image.setPixelVal(i, j, val);     
      }

    delete [] charImage;

    return (1);
}

int ImageParser::WriteImage(std::string fname, Image& image)
{
    int i, j;
    int N, M, Q;
    unsigned char *charImage;
    ofstream ofp;

    image.getImageInfo(N, M, Q);

    charImage = (unsigned char *) new unsigned char [M*N];

    // convert the integer values to unsigned char

    int val;

    for(i=0; i<N; i++)
    {
        for(j=0; j<M; j++) {
            image.getPixelVal(i, j, val);
            charImage[i*M+j]=(unsigned char)val;
        }
    }

    ofp.open(fname, ios::out | ios::binary);

    if (!ofp) {
        cout << "Can't open file: " << fname << endl;
        exit(1);
    }

    ofp << "P5" << endl;
    ofp << M << " " << N << endl;
    ofp << Q << endl;

    ofp.write( reinterpret_cast<char *>(charImage), (M*N)*sizeof(unsigned char));

    if (ofp.fail()) {
      cout << "Can't write image " << fname << endl;
      exit(0);
    }

    ofp.close();

    delete [] charImage;

    return(1);
}

int ImageParser::ReadImageHeader(std::string fname, int& N, int& M, int& Q, bool& type)
{
    int i, j;
    unsigned char *charImage;
    char header [100], *ptr;
    ifstream ifp;

    ifp.open(fname, ios::in | ios::binary);

    if (!ifp) {
      cout << "Can't read image: " << fname << endl;
      exit(1);
    }

    // read header

    type = false; // PGM

    ifp.getline(header,100,'\n');
    if ( (header[0] == 80) &&  /* 'P' */
         (header[1]== 53) ) {  /* '5' */
         type = false;
    }
    else if ( (header[0] == 80) &&  /* 'P' */
         (header[1] == 54) ) {        /* '6' */
         type = true;
    } 
    else {
      cout << "Image " << fname << " is not PGM or PPM" << endl;
      exit(1);
    }

    ifp.getline(header,100,'\n');
    while(header[0]=='#')
      ifp.getline(header,100,'\n');

    M=strtol(header,&ptr,0);
    N=atoi(ptr);

    ifp.getline(header,100,'\n');

    Q=strtol(header,&ptr,0);

    ifp.close();

    return(1);
}



Image::Image()
{
 N = 0;
 M = 0;
 Q = 0;
 for (int i = 0; i < NUM_COLOURS; i++)
 {
    pixelValue[i] = NULL;
 }
}

Image::Image(int tmpN, int tmpM, int tmpQ)
{
    int i, j;

    N = tmpN;
    M = tmpM;
    Q = tmpQ;
    for (int c = 0; c < NUM_COLOURS; c++)
    {
        pixelValue[c] = new int* [N];
        for(i=0; i<N; i++) {
        pixelValue[c][i] = new int[M];
        for(j=0; j<M; j++)
            pixelValue[c][i][j] = 0;
        }
    }
}

Image::Image(Image& oldImage)
{
    int i, j;

    N = oldImage.N;
    M = oldImage.M;
    Q = oldImage.Q;
    
    for (int c = 0; c < NUM_COLOURS; c++)
    {
        pixelValue[c] = new int* [N];
        for(i=0; i<N; i++) {
            pixelValue[c][i] = new int[M];
            for(j=0; j<M; j++)
                pixelValue[c][i][j] = oldImage.pixelValue[c][i][j];
       }
    }
}

Image::~Image()
{
    int i;

    for (int c = 0; c < NUM_COLOURS; c++)
    {
        for(i=0; i<N; i++)
        {
            delete [] pixelValue[c][i];
        }
        delete [] pixelValue[c];
    }
}


void Image::getImageInfo(int& rows, int& cols, int& levels)
{
 rows = N;
 cols = M;
 levels = Q;
} 

void Image::setImageInfo(int rows, int cols, int levels)
{
 N= rows;
 M= cols;
 Q= levels;
} 

void Image::setPixelVal(int c, int i, int j, int val)
{
 pixelValue[c][i][j] = val;
}

void Image::getPixelVal(int c, int i, int j, int& val)
{
 val = pixelValue[c][i][j];
}

