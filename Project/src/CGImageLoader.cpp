/*
 TCS2221 Computer Graphics
 Trimester II, 2015/16
 Faculty of IT, Multimedia University

 CGImageLoader.cpp

 Objective: Image Loader to load images for texture
            (So far only JPG and BMP files are supported)

 This file (CGImageLoader.cpp) can be distributed to the students

 INSTRUCTIONS
 ============
 Please refer to CGImageLoader.hpp for instructions

 SPECIAL NOTES
 =============

 CHANGE LOG
 ==========

 TO DO
 =====
*/
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <GL/glut.h>
#include <iostream>
#include "CGimageloader.hpp"

#ifdef _WIN32
#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <float.h>
#endif

//BEGIN Main Loader Wrapper class
MyImage::MyImage()
{
 width=0; height=0;
 bmpimage.Width = 0;
 bmpimage.Height = 0;
 bmpimage.Pixels = NULL;
 buffer = NULL;
}

MyImage::~MyImage()
{
 freeMemory();
}

void MyImage::loadBMP(char* filename)
{
 freeMemory();
 bmpimage.Width = 0;
 bmpimage.Height = 0;
 bmpimage.Pixels = NULL;
 ReadBMP(filename,&bmpimage);
 width = bmpimage.Width;
 height = bmpimage.Height;
 buffer = bmpimage.Pixels;
}

void MyImage::loadJPG(char* filename)
{
 freeMemory();
 jpgimage.readJpeg(filename);  //will auto clear buffer
 width = jpgimage.getWidth();
 height = jpgimage.getHeight();
 buffer = jpgimage.getValuePtr();
}

void MyImage::freeMemory()
{
 if (bmpimage.Pixels!=NULL)
 {
    free (bmpimage.Pixels);
 }
 bmpimage.Pixels = NULL;
 bmpimage.Width = 0;
 bmpimage.Height = 0;
 width = 0;
 height = 0;
 buffer = NULL;
}
//END Main Loader Wrapper class

namespace YPImageLoaders
{
#ifndef TRUE
#define TRUE   1
#define FALSE  0
#endif

//BEGIN BMP Loader ------------------
/*=============`=============================================================
  Task:	Read a Microsoft defined BMP (Bitmap) file.
============================================================================*/
typedef struct BMPFileHeader
{
 unsigned long FileSize;
 unsigned long Reserved1;
 unsigned long BitmapDataOffset;
} BMPFileHeader;

typedef struct BMPInfo
{
 unsigned long	HeaderSize; // in bytes
 unsigned long	Width;
 unsigned long	Height;
 unsigned short	NumberColorPlanes; // must be 1
 unsigned short	BitsPerPixel;
 unsigned long	CompressionType;	//0: no compression;
                                    //1: run length (8 bits per pixel)
                                    //2: run length (4 bits per pixel)
 unsigned long	BytesInImage;
 unsigned long	HorizontalResolution; /* in pixels per meter */
 unsigned long	VerticalResolution;
 unsigned long	ColorsUsedFromTable;
 unsigned long	ColorsRequiredFromTable;
} BMPInfo;

typedef struct BMPColorEntry
{
 unsigned char	Blue;
 unsigned char 	Green;
 unsigned char	Red;
 unsigned char	Reserved;
} BMPColorEntry;

static int SwapByteOrder;

static void SwapLongInt( unsigned long int *L );
static void SwapShortInt( unsigned short int *S );
static void SetSwapEndian();

//-------------------------------------------------------------------
static void SwapLongInt( unsigned long int *L )
{
 *L =   (*L >> 24) | ((*L >> 8) & 0x0000FF00)
      | (*L << 24) | ((*L << 8) & 0x00FF0000);
}
//-------------------------------------------------------------------
static void SwapShortInt( unsigned short int *S )
{
 *S = (*S >> 8) | (*S << 8);
}
/*-------------------------------------------------------------------------*/
static void SetSwapEndian()
{
 short			x = 1;
 char			*xbyte = (char *) &x;

 // Note: a BMP file is always stored in Little Endian byte order
 //       if this machine is Big Endian, then all integers must be swapped
  if (*xbyte == 0) // this is a BigEndian machine
 	 SwapByteOrder = TRUE;
  else
     SwapByteOrder = FALSE;
}

#ifdef _WIN32
static bool gGdiplusInitialized = false;
static ULONG_PTR gGdiplusToken = 0;

static void EnsureGdiplusInitialized() {
    if (!gGdiplusInitialized) {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        if (Gdiplus::GdiplusStartup(&gGdiplusToken, &gdiplusStartupInput, NULL) == Gdiplus::Ok) {
            _fpreset();
            gGdiplusInitialized = true;
        }
    }
}

static bool ReadBMPWithGdiPlus(const char* filename, YPImageLoaders::MyImageStruct* Image) {
    EnsureGdiplusInitialized();
    if (!gGdiplusInitialized || Image == NULL) {
        return false;
    }

    // Convert filename to wide string
    int len = MultiByteToWideChar(CP_ACP, 0, filename, -1, NULL, 0);
    if (len <= 0) return false;
    std::vector<wchar_t> widePath((size_t)len, L'\0');
    if (MultiByteToWideChar(CP_ACP, 0, filename, -1, &widePath[0], len) <= 0) {
        return false;
    }

    Gdiplus::Bitmap bitmap(&widePath[0]);
    if (bitmap.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    const int width = (int)bitmap.GetWidth();
    const int height = (int)bitmap.GetHeight();
    if (width <= 0 || height <= 0) {
        return false;
    }

    Gdiplus::Rect rect(0, 0, width, height);
    Gdiplus::BitmapData locked = {};
    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &locked) != Gdiplus::Ok) {
        return false;
    }

    std::vector<unsigned char> rgb((size_t)width * (size_t)height * 3u);
    if (rgb.empty()) {
        bitmap.UnlockBits(&locked);
        return false;
    }

    for (int y = 0; y < height; ++y) {
        const unsigned char* srcRow = (const unsigned char*)locked.Scan0 + y * locked.Stride;
        unsigned char* dstRow = &rgb[(size_t)(height - 1 - y) * (size_t)width * 3u];
        for (int x = 0; x < width; ++x) {
            const unsigned char b = srcRow[x * 4 + 0];
            const unsigned char g = srcRow[x * 4 + 1];
            const unsigned char r = srcRow[x * 4 + 2];
            dstRow[x * 3 + 0] = r;
            dstRow[x * 3 + 1] = g;
            dstRow[x * 3 + 2] = b;
        }
    }

    bitmap.UnlockBits(&locked);

    Image->Width = width;
    Image->Height = height;
    Image->Pixels = (unsigned char*)malloc((size_t)width * (size_t)height * 3u);
    if (Image->Pixels == NULL) {
        Image->Width = 0;
        Image->Height = 0;
        return false;
    }
    memcpy(Image->Pixels, &rgb[0], (size_t)width * (size_t)height * 3u);
    return true;
}
#endif

//-------------------------------------------------------------------------
void ReadBMP(char *FileName, MyImageStruct *Image)
{
 FILE   		*fp;
 char			MagicNumber[2];
 BMPFileHeader	FileHeader;
 BMPInfo		Info;
 short			status;
 int			Debug = FALSE;
 BMPColorEntry	ColorTable[256];
 int            ColorTableSize;
 int			BytesPerRow;
 int			TotalBytesPerRow;
 int			BytesPerPixel;
 int			x, y;
 int			Index;
 unsigned char	*p;

 // Determine whether to swap the byte order (Little vs. Big Endian) or not
 SetSwapEndian();

 // Open the file
 fp = fopen(FileName,"rb");
 if (fp == NULL) { printf("Could not open file '%s'\n", FileName); return; }

 // read the file "magic number"
 status = fread( &MagicNumber, 2, 1, fp);
 if (MagicNumber[0] != 'B' || MagicNumber[1] != 'M')
    printf("Error reading BMP file, file type incorrect\n");
 else
 {
  	// read the file header
  	status = fread( &FileHeader, sizeof(BMPFileHeader), 1, fp);
	if (SwapByteOrder) SwapLongInt(&FileHeader.FileSize);
	if (SwapByteOrder) SwapLongInt(&FileHeader.BitmapDataOffset);
  	if (Debug)
       printf("Size = %lu\nOffset = %lu\n",
         		FileHeader.FileSize,
                FileHeader.BitmapDataOffset);
  	if (status <= 0) printf("Error reading BMP file header\n");
  	else
  	{
 	   // read the image header
  	   status = fread( &Info, sizeof(BMPInfo), 1, fp);
	   if (SwapByteOrder) SwapLongInt(&Info.HeaderSize);
       if (SwapByteOrder) SwapLongInt(&Info.Width);
       if (SwapByteOrder) SwapLongInt(&Info.Height);
       if (SwapByteOrder) SwapShortInt(&Info.NumberColorPlanes);
       if (SwapByteOrder) SwapShortInt(&Info.BitsPerPixel);
       if (SwapByteOrder) SwapLongInt(&Info.CompressionType);
       if (Debug)
  		  printf(	"Header size = %lu\nWidth = %lu\nHeight = %lu\nNumberColorPlanes = %hu\n"
  							"BitsPerPixel = %hu\nCompressionType = %lu\n",
          			Info.HeaderSize, Info.Width, Info.Height,
          			Info.NumberColorPlanes, Info.BitsPerPixel, Info.CompressionType);
  	   if (status <= 0) printf("Error reading BMP Information record\n");
  	   else
  	   {
   		  if (Info.CompressionType != 0) {
             printf("Warning: Compressed BMP '%s'. Trying GDI+ fallback...\n", FileName);
#ifdef _WIN32
             fclose(fp);
             if (ReadBMPWithGdiPlus(FileName, Image)) {
                 return;
             }
             printf("GDI+ fallback failed for compressed BMP '%s'\n", FileName);
             return;
#else
             printf("Sorry, this code cannot read compressed image files\n");
#endif
          }
          else
          {
             switch (Info.BitsPerPixel)
  			 {
  			    case 8:  ColorTableSize = 256;	BytesPerPixel = 1;	break;
  			    case 24: ColorTableSize = 0;		BytesPerPixel = 3;	break;
                default: BytesPerPixel = 0;
                         printf("Warning: BMP '%s' is not 8-bit or 24-bit. Trying GDI+ fallback...\n", FileName);
#ifdef _WIN32
                         fclose(fp);
                         if (ReadBMPWithGdiPlus(FileName, Image)) {
                             return;
                         }
                         printf("GDI+ fallback failed for non-standard BMP '%s'\n", FileName);
                         break;
#else
                         printf("Sorry, this code can only read 8 bit or 24 bit image files\n");
                         break;
#endif
  			 }
  			 if (Debug)
  			    printf("ColorTableSize = %d\nBytesPerPixel = %d\n", ColorTableSize, BytesPerPixel);

  			 if (BytesPerPixel > 0)
             {
                //Read the color pallete (if there is one)
  			    if (ColorTableSize > 0)
  			    {
  			       status = fread( &ColorTable, sizeof(BMPColorEntry), ColorTableSize, fp);
  				   if (status <= 0) printf("Error reading BMP Color Table\n");
  			    }

				// Setup the Image data record and allocate memory for the image.
                //A full color, 24-bit per pixel image is always created.
				Image->Width  = Info.Width;
				Image->Height = Info.Height;
				Image->Pixels = (unsigned char *) malloc( Image->Width * Image->Height * 3 );
				if (Image->Pixels == NULL)
				   printf("Error allocating pixel memory\n");
				else
				{
  				   // Read the pixel data
  				   fseek( fp, FileHeader.BitmapDataOffset, SEEK_SET);
  				   TotalBytesPerRow = BytesPerRow = Info.Width * BytesPerPixel;
  				   if (TotalBytesPerRow % 4 != 0)
  				      TotalBytesPerRow += (4 - TotalBytesPerRow % 4); /* round to 32 bit alignment */
  				   if (Debug) printf("BytesPerRow = %d\n", TotalBytesPerRow);

  				   unsigned char* PixelRow = (unsigned char*) malloc(TotalBytesPerRow);
  				   if (PixelRow == NULL) {
  				      printf("Error allocating row buffer memory\n");
  				   } else {
  				      p = Image->Pixels;
  				      for (y = 0; y < Info.Height; y++)
  				      {
  			  	         status = fread( PixelRow, TotalBytesPerRow, 1, fp);
  					     if (status <= 0) { printf("Error reading BMP pixels\n"); break; }

  					     for (x = 0; x < BytesPerRow; )
  					     {
  				  	        if (BytesPerPixel == 1)
  				  	        {
  				  	           Index = PixelRow[x++];
  				  	           *p++ = ColorTable[Index].Red;
  				  	           *p++ = ColorTable[Index].Green;
  				  	           *p++ = ColorTable[Index].Blue;
  				  	        }
  				  	        else // BytesPerPixel == 3
  				  	        {
  				  	           *p++ = PixelRow[x+2]; /* green */
  				  	           *p++ = PixelRow[x+1];	/* blue  */
  				  	           *p++ = PixelRow[x];		/* red	 */
  				  	           x += 3;
  				  	        }
  					     }
  				      }
  				      free(PixelRow);
  				   }
                }
             }
  		  }
  	   }
  	}
 }
 fclose(fp);
}
//END BMP Loader ------------------

//BEGIN Jpeg Loader Wrapper class
//      based on jpeg loader by Independent JPEG Group
//      url: http://www.ijg.org/
//copy constructor
Image::Image(const Image &img)
{
 component_count = img.component_count;
 width = img.width;
 height = img.height;
 color_space = img.color_space;
 const unsigned int total = height * width * component_count;
 value = new ubyte[total];
 for(unsigned int i = 0; i < total; ++i)
    value[i] = img.value[i];
}

//assignment operator
Image &Image::operator=(const Image &img)
{
 if(&img == this) return *this;
 clear();
 component_count = img.component_count;
 width = img.width;
 height = img.height;
 color_space = img.color_space;
 const unsigned int total = height * width * component_count;
 value = new ubyte[total];
 for (unsigned int i = 0; i < total; ++i)
    value[i] = img.value[i];
 return *this;
}

//jpeg error handling
void Image::jpegErrorExit(j_common_ptr cinfo)
{
 JpegErrorManager *error = (JpegErrorManager*)cinfo->err;
 (*cinfo->err->output_message) (cinfo);
 longjmp(error->setjmp_buffer, 1);
}

//read jpeg file, return true if success, false otherwise
bool Image::readJpeg(const char *filename)
{
 jpeg_decompress_struct cinfo;
 JpegErrorManager jerr;
 FILE *infile;

 if( !(infile = fopen(filename, "rb")) )
 {
	std::cerr << "Cannot open " << filename << "\n";
	return false;
 }

 cinfo.err = jpeg_std_error(&jerr.pub);
 jerr.pub.error_exit = jpegErrorExit;

 if(setjmp(jerr.setjmp_buffer))
 {
	std::cerr << "JPEG decompress error\n";
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return false;
 }

 jpeg_create_decompress(&cinfo);
 jpeg_stdio_src(&cinfo, infile);
 jpeg_read_header(&cinfo, true);
 jpeg_start_decompress(&cinfo);

 width = cinfo.output_width;
 height = cinfo.output_height;
 component_count = cinfo.output_components;
 color_space = static_cast<ColorSpace>(cinfo.out_color_space);

 value = new ubyte[height * width * component_count];
 const unsigned int rowstride = width * component_count;
 ubyte *scanline[1];
 for(unsigned int h = 0; h < height; ++h)
 {
    scanline[0] = value + h*rowstride;
    jpeg_read_scanlines(&cinfo, (JSAMPARRAY)(scanline), 1);
 }

 jpeg_finish_decompress(&cinfo);
 jpeg_destroy_decompress(&cinfo);
 fclose(infile);

 if(jerr.pub.num_warnings > 0)
 {
    std::cerr << "WARNING! Corrupted data may occurred\n";
 }

 return true;
}

//write jpeg file, return true if success, false otherwise
//quality ranges from 0 to 100, higher value better quality
bool Image::writeJpeg(const char *filename, const int quality)
{
 if(!value)
 {
    std::cerr << "Nothing to be written\n";
    return false;
 }

 jpeg_compress_struct cinfo;
 JpegErrorManager jerr;
 FILE *outfile;

 cinfo.err = jpeg_std_error(&jerr.pub);
 jerr.pub.error_exit = jpegErrorExit;

 jpeg_create_compress(&cinfo);

 if( !(outfile = fopen(filename, "wb")) )
 {
	std::cerr << "Cannot open " << filename << "\n";
	return false;
 }

 if(setjmp(jerr.setjmp_buffer))
 {
	std::cerr << "JPEG compress error\n";
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);
	return false;
 }

 jpeg_stdio_dest(&cinfo, outfile);
 cinfo.image_width = width;
 cinfo.image_height = height;
 cinfo.input_components = component_count;
 cinfo.in_color_space = static_cast<J_COLOR_SPACE>(color_space);
 jpeg_set_defaults(&cinfo);
 quality < 0 ? jpeg_set_quality(&cinfo, 0, true) :
 quality > 100 ? jpeg_set_quality(&cinfo, 100, true) :
                 jpeg_set_quality(&cinfo, quality, true);
 jpeg_start_compress(&cinfo, true);

 const unsigned int rowstride = width * component_count;
 ubyte *scanline[1];
 for(unsigned int h = 0; h < height; ++h)
 {
    scanline[0] = value + h*rowstride;
	jpeg_write_scanlines(&cinfo, (JSAMPARRAY)(scanline), 1);
 }

 jpeg_finish_compress(&cinfo);
 fclose(outfile);
 jpeg_destroy_compress(&cinfo);

 return true;
}
//END Jpeg Loader Wrapper class

};

