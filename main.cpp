// The MIT License (MIT)
// 
// Copyright (c) 2013 Gerard Meier
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <unistd.h>

#include "libs/ati/TextureConverter.h"
#include "libs/libPNG/png.h"

using std::cout;
using std::endl;
using std::string;

struct KtxFormat {
    /// Constructor is only required when creating KTX images.
    KtxFormat() :
        endianness(0), glType(0), glTypeSize(0), glFormat(0), glInternalFormat(0),
        glBaseInternalFormat(0), pixelWidth(0), pixelHeight(0), pixelDepth(0),
        numberOfArrayElements(0), numberOfFaces(0), numberOfMipmapLevels(0), bytesOfKeyValueData(0) {
        
        // As per http://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
        identifier[0]  = 0xAB;
        identifier[1]  = 0x4B;
        identifier[2]  = 0x54;
        identifier[3]  = 0x58;
        identifier[4]  = 0x20;
        identifier[5]  = 0x31;
        identifier[6]  = 0x31;
        identifier[7]  = 0xBB;
        identifier[8]  = 0x0D;
        identifier[9]  = 0x0A;
        identifier[10] = 0x1A;
        identifier[11] = 0x0A;
    }
    
    unsigned char identifier[12];
    uint32_t endianness;
    uint32_t glType;
    uint32_t glTypeSize;
    uint32_t glFormat;
    uint32_t glInternalFormat;
    uint32_t glBaseInternalFormat;
    uint32_t pixelWidth;
    uint32_t pixelHeight;
    uint32_t pixelDepth;
    uint32_t numberOfArrayElements;
    uint32_t numberOfFaces;
    uint32_t numberOfMipmapLevels;
    uint32_t bytesOfKeyValueData;
};

void error(const std::string& wut) {
    cout << "Error: " << wut << endl;
    cout << "Usage: aticompress input.png output.ktc" << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, const char * argv[]) {

    if(argc < 2) {
        error("No input file specified.");
    }
    
    if(argc < 3) {
        error("No output file specified.");
    }
    
    const string filenameIn(argv[1]);
    const string filenameOut(argv[2]);


    FILE* fp = fopen(filenameIn.c_str(), "r");

    if(fp == 0) {
        error("Input file not found: " + filenameIn);
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr  = png_create_info_struct(png_ptr);

    if (setjmp(png_jmpbuf(png_ptr))) {
        error("Whatever just happend wasn't good.");
    }
    
    png_init_io(png_ptr, fp);
    
    unsigned int sig_read = 0;
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
    
    
    png_uint_32 width, height; int bit_depth, color_type, interlace_type;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);
    
    if(color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGBA) {
        error("Only RGB and RGBA are supported. None of that greyscale luminance stuff.");
    }
    
    unsigned long row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    unsigned char* data = (unsigned char*) malloc(row_bytes * height);

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    
    for (unsigned int i = 0; i < height; i++) {
        memcpy(data+(row_bytes * i), row_pointers[i], row_bytes);
    }
    
    cout << "Bit depth: " << bit_depth << ", width: " << width
         << ", height: " << height << ", alpha: " << std::boolalpha << ((color_type==PNG_COLOR_TYPE_RGBA)?true:false) << ", ";

    
    TQonvertImage qualcommTextureInput;
    
    qualcommTextureInput.pFormatFlags = (TFormatFlags*) malloc(sizeof(TFormatFlags));
    memset(qualcommTextureInput.pFormatFlags, 0, sizeof(TFormatFlags));
    
    qualcommTextureInput.nWidth     = width;
    qualcommTextureInput.nHeight    = height;
    qualcommTextureInput.nFormat    = (color_type == PNG_COLOR_TYPE_RGB) ? Q_FORMAT_RGB_888 : Q_FORMAT_RGBA_8888;
    qualcommTextureInput.nDataSize  = width * height * bit_depth / 8;
    qualcommTextureInput.pFormatFlags->nMaskRed     = 0x0000FF;
    qualcommTextureInput.pFormatFlags->nMaskGreen   = 0x00FF00;
    qualcommTextureInput.pFormatFlags->nMaskBlue    = 0xFF0000;
    qualcommTextureInput.pFormatFlags->nFlipY       = 0;
    qualcommTextureInput.pData                      = (unsigned char*) data;
        
    TQonvertImage qualcommTextureOutput;
    qualcommTextureOutput.pFormatFlags = (TFormatFlags*)malloc(sizeof(TFormatFlags));
    memset(qualcommTextureOutput.pFormatFlags, 0, sizeof(TFormatFlags));
    
    qualcommTextureOutput.nWidth    = qualcommTextureInput.nWidth;
    qualcommTextureOutput.nHeight   = qualcommTextureInput.nHeight;
    qualcommTextureOutput.nFormat   = (color_type == PNG_COLOR_TYPE_RGB) ? Q_FORMAT_ATITC_RGB : Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA;
    qualcommTextureOutput.nDataSize = 0;
    qualcommTextureOutput.pData     = NULL;
    
    if(Qonvert(&qualcommTextureInput, &qualcommTextureOutput) != Q_SUCCESS) {
        error("The first Qonvert call failed.");
    }
    qualcommTextureOutput.pData = (unsigned char*) malloc(qualcommTextureOutput.nDataSize);
    
    if(Qonvert(&qualcommTextureInput, &qualcommTextureOutput) != Q_SUCCESS) {
        error("The first Qonvert call failed.");
    }

    // http://www.khronos.org/registry/gles/extensions/AMD/AMD_compressed_ATC_texture.txt

    KtxFormat ktx;
    ktx.pixelWidth           = width;
    ktx.pixelHeight          = height;
    ktx.pixelDepth           = bit_depth;
    ktx.numberOfMipmapLevels = 1;
    
    switch(qualcommTextureOutput.nFormat) {
        case Q_FORMAT_ATITC_RGB:
            ktx.glInternalFormat = 0x8C92; // ATC_RGB_AMD
            cout << "outfile: '" << filenameOut << "', format: Q_FORMAT_ATITC_RGB";
            break;
        case Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA:
            ktx.glInternalFormat = 0x8C93; // ATC_RGBA_EXPLICIT_ALPHA_AMD
            cout << "outfile: '" << filenameOut << "', format: Q_FORMAT_ATC_RGBA_EXPLICIT_ALPHA";
            break;
        case Q_FORMAT_ATC_RGBA_INTERPOLATED_ALPHA:
            ktx.glInternalFormat = 0x87EE; // ATC_RGBA_INTERPOLATED_ALPHA_AMD
            cout << "outfile: '" << filenameOut << "', format: Q_FORMAT_ATC_RGBA_INTERPOLATED_ALPHA";
            break;
        default:
            error("Internal error, no output format specified.");
            break;
    }

    // Remove the old file
    remove(filenameOut.c_str());

    FILE* out = fopen(filenameOut.c_str(), "w");
    
    // Write the header:
    fwrite((void*) &ktx, 1, sizeof(KtxFormat), out);
    
    // Write the data size:
    fwrite((void*) &(qualcommTextureOutput.nDataSize), 1, sizeof(unsigned int), out);

    // Write actual data:
    fwrite(qualcommTextureOutput.pData, 1, qualcommTextureOutput.nDataSize, out);
    
    cout << " - done (" << (sizeof(KtxFormat) + sizeof(unsigned int) + qualcommTextureOutput.nDataSize) << " bytes) !" << endl;
    return EXIT_SUCCESS;
}
