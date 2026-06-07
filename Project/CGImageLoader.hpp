/*
 TCS2221 Computer Graphics
 Trimester II, 2015/16
 Faculty of IT, Multimedia University

 CGImageLoader.hpp

 Objective: Header file for Image Loader to load images for texture
            (So far only JPG and BMP files are supported)

 This file (CGImageLoader.hpp) can be distributed to the students

 INSTRUCTIONS
 ============
 * Files required:
   Header files : CGImageLoader.hpp (this file)
                  jpeglib.h
                  jconfig.h
                  jmorecfg.h
   Cpp file     : CGImageLoader.cpp
   Library files: libjpeg.dll.a
                  jpeg62.dll
 * Steps:
   a. Create a OpenGL project file as usual
   b. Copy the library files to the compiler 'lib' folder,
      or you can copy them to any folder and then specify the location
      by going to Build Option->Directories->Linker and type in the directory
      name. If you store the library files at the same folder as your program, then
      specify the director as '.' (just a dot, meaning current directory)
   c. Make sure jpeg62.dll either in the windows\system32 folder or at the folder
      where your exe file is.
   d. Copy the CGImageLoader.cpp to where your main program is,
      include it to your project.
   e. You may put the headers file either in your compiler include folder
      or put them in any folder but specify the folder
      at Build Option->Directories->Compiler. You may put them in the same folder
      as your program as well.
   f. To load an image, just use one of the following:
          MyImage image;
          image.loadBMP("test1.bmp");
          image.loadJPG("test2.jpg");

 Comments:
 * This image loader wraps the image loaders by:
    a.  Image loader for BMP image file
    b.  Image loader for JPG image file
 * So far only BMP and JPG are supported
 * Take note BMP loader loads the images from bottom to top,
       while JPG loader loads the images from top to bottom
 * IMPORTANT: For use of texture, your image dimension must be multiples of 2,
              meaning that the size must be 2^n

 CHANGE LOG
 ==========
*/

#include <string>
#include "jpeglib.h"
#include <setjmp.h>

namespace YPImageLoaders {

//BEGIN BMP Loader ------------------
typedef struct MyImageStruct
{
 unsigned long Width;
 unsigned long Height;
 unsigned char *Pixels; //Always RGB, row major order
} MyImageStruct;

void ReadBMP(char *FileName, MyImageStruct *Image);
//END BMP Loader ------------------

//BEGIN Jpeg Loader Wrapper class

typedef unsigned char ubyte;

class Image
{
 public:
    //color space enumuration
    enum ColorSpace { Unknown, Grayscale, RGB, YCbCr, CMYK, YCCK };

    //constructor
    Image()
      : value(0), component_count(0), width(0), height(0),
        color_space(Unknown) {}

    //alternative constructor
    Image(const char *filename) :
          value(0), component_count(0), width(0), height(0),
          color_space(Unknown) { readJpeg(filename); }

    //alternative constructor
    Image(const std::string &filename) :
          value(0), component_count(0), width(0), height(0),
          color_space(Unknown) { readJpeg(filename.c_str()); }

    //copy constructor
    Image(const Image &img);

    //destructor
    ~Image() { clear(); }

    //assignment operator
    Image &operator=(const Image &img);

    //read jpeg file
    bool readJpeg(const char *filename);

    //read jpeg file overloaded for convenience
    bool readJpeg(const std::string &filename)
    {
       return readJpeg(filename.c_str());
    }

    //write jpeg file, quality parameter ranges from 0 to 100
    bool writeJpeg(const char *filename, const int quality);

    //write jpeg file, overloaded for convenience
    bool writeJpeg(const std::string &filename, const int quality)
    {
       return writeJpeg(filename.c_str(), quality);
    }

    //get the pointer to value
    ubyte * const getValuePtr() const { return value; }

    //get component count
    const unsigned int getComponentCount() const { return component_count; }

    //get width
    const unsigned int getWidth() const { return width; }

    //get height
    const unsigned int getHeight() const { return height; }

    //get color space
    const ColorSpace getColorSpace() const { return color_space; }

 private:
    ubyte *value;                  //the pixel value, 1 byte per component
    unsigned int component_count;   //number of components (ie RGB = 3)
    unsigned int width;             //the width of the image, in pixels
    unsigned int height;            //the height of the image, in pixels
    ColorSpace color_space;         //the color space of the image

    //jpeg error manager wrapper
    struct JpegErrorManager
    {
     struct jpeg_error_mgr pub;
            jmp_buf setjmp_buffer;
    };

    //jpeg error handling
    METHODDEF(void) jpegErrorExit(j_common_ptr cinfo);

    //clear allocated memory
    void clear() { if(value) delete[] value; }
};
//END Jpeg Loader Wrapper class

};

//BEGIN Main Loader Wrapper class
class MyImage
{
 public:
    MyImage();
    ~MyImage();
    unsigned int width;
    unsigned int height;
    unsigned char *buffer;
    void loadBMP(char* filename);
    void loadJPG(char* filename);
 private:
    YPImageLoaders::MyImageStruct bmpimage;  //for BMP loader
    YPImageLoaders::Image jpgimage;  //for Jpg loader wrapper
    void freeMemory();
};
//END Main Loader Wrapper class
