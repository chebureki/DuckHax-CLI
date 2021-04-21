#include "bitmapparser.h"
#include "errormessages.h"

#include <png.h>
#include <stdio.h>
#include <string>
#include <stdexcept>
#include <stdio.h>
#include <iostream>

BitmapParser::BitmapParser(){}
BitmapParser::~BitmapParser(){}

void BitmapResult::freeData(png_bytepp data, int height){
    for(int i=0;i<height;i++)
        free(data[i]);
    free(data);
}

BitmapResult *BitmapParser::parseFile(std::string pathIn,CRC32Lookup crcLookup){

    FILE *file = fopen(pathIn.c_str(),"rb");
    if (file == nullptr){
         throw CANT_READ(pathIn.c_str());
    }
    fseek(file,4,SEEK_SET); //ignore the file-size!
    uint32_t fileMagic;
    fread(&fileMagic,4,1,file);
    if (crcLookup.getClass(fileMagic) != ZounaClasses::Bitmap_Z)
        throw std::invalid_argument(pathIn+" is not a bitmap file!");
    uint32_t nameCRC;
    fread(&nameCRC,4,1,file);
    BitmapResult *result = new BitmapResult(pathIn,nameCRC);
    fread(&result->uCRC2,4,1,file);
    fread(&result->m_width,4,1,file);
    fread(&result->m_height,4,1,file);
    fread(&result->u1,4,1,file);
    fread(&result->m_type,1,1,file);
    fread(&result->u2,4,1,file);
    fseek(file,1,SEEK_CUR); // Skip the 0x15 byte
    result->m_pixels = new uint16_t[result->m_width*result->m_height];
    fread(result->m_pixels,2,result->m_width*result->m_height,file);
    fclose(file);
    return result;
}

std::string BitmapResult::inspectSpecific(){
    //TODO: fix this formatting monstrosity!
    std::string output = "";
    char dst[32];
    sprintf(dst, "Dimensions: %d x %d\nType: %d\n",m_width,m_width,m_type);
    return output+dst;
}

void BitmapResult::dump(std::string pathOut){
    FILE *file = fopen(pathOut.c_str(),"wb");
    if(file == nullptr)
        throw CANT_READ(pathOut.c_str());

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if(png == nullptr)
        throw std::runtime_error("Error in libpng, can't create png_structp");

    png_infop pngInfo = png_create_info_struct(png);
    if(pngInfo == nullptr)
        throw std::runtime_error("Error in libpng, can't create png_infop");

    if (setjmp(png_jmpbuf(png)))
        throw std::runtime_error("Error in libpng, can't set jmp for write");

    png_init_io(png,file);
    png_set_IHDR(
                 png,
                 pngInfo,
                 m_width,
                 m_height,
                 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT
                 );
    png_write_info(png,pngInfo);

    png_bytepp data;
    switch(m_type){
    case 0xA:
        data = generateRGBA();
        break;
    case 0x7:
        data = generateMonoAlphaRGBFull();
        break;
    case 0x8:
        data = generateRGB();
        break;

    default:
        throw std::invalid_argument(std::to_string(m_type)+" is (currently) unsupported bitmap-type!");
    }
    png_write_image(png,data);
    png_write_end(png, nullptr);
    png_destroy_write_struct(&png,&pngInfo);
    fclose(file);
    freeData(data,m_height);
}

BitmapResult::BitmapResult(std::string pathIn, uint32_t nameCRC): ParserResult(ZounaClasses::Bitmap_Z,nameCRC,pathIn){};
BitmapResult::~BitmapResult(){
    delete[] m_pixels;
};

uint8_t BitmapResult::getBitmapType(){
    return m_type;
}

png_bytepp BitmapResult::generateRGBA(){
    const int rowbytes = m_width * 4;
    png_bytepp row_ptr = (png_bytepp)malloc(sizeof(png_bytep) * m_height);

    for (uint32_t y=0; y<m_height; ++y){
        row_ptr[y] = (png_bytep)malloc(rowbytes);
    }

    for (uint32_t y=0; y<m_height; ++y){
        png_bytep row = row_ptr[y];

        for (uint32_t x=0; x<m_width; ++x){
            uint16_t pixel = m_pixels[(y*m_width) + x];
            png_bytep p = &row[x*4];
            p[0] =  ((pixel&0x0f00)>>8)*16; //r
            p[1] =  ((pixel&0x00f0)>>4)*16; //g
            p[2] =  ((pixel&0x000f)>>0)*16; //b
            p[3] =  ((pixel&0xf000)>>12)*16;//a
        }
    }
    return row_ptr;
}

png_bytepp BitmapResult::generateMonoAlphaRGBFull(){
    const int rowbytes = m_width * 4;
    png_bytepp row_ptr = (png_bytepp)malloc(sizeof(png_bytep) * m_height);

    for (uint32_t y=0; y<m_height; ++y){
        row_ptr[y] = (png_bytep)malloc(rowbytes);
    }

    for (uint32_t y=0; y<m_height; ++y){
        png_bytep row = row_ptr[y];

        for (uint32_t x=0; x<m_width; ++x){
            uint16_t pixel = m_pixels[(y*m_width) + x];
            png_bytep p = &row[x*4];
            p[0] =  ((pixel&0x7C00)>>10) * 8; //r
            p[1] =  ((pixel&0x03E0)>>5) * 8; //g
            p[2] =  ((pixel&0x001F)>>0) * 8; //b
            p[3] = ((pixel&0x8000)>>15)*255; //a
        }
    }
    return row_ptr;
}

png_bytepp BitmapResult::generateRGB(){
    const int rowbytes = m_width * 4;
    png_bytepp row_ptr = (png_bytepp)malloc(sizeof(png_bytep) * m_height);

    for (uint32_t y=0; y<m_height; ++y){
        row_ptr[y] = (png_bytep)malloc(rowbytes);
    }

    for (uint32_t y=0; y<m_height; ++y){
        png_bytep row = row_ptr[y];

        for (uint32_t x=0; x<m_width; ++x){
            uint16_t pixel = m_pixels[(y*m_width) + x];
            png_bytep p = &row[x*4];
            p[0] =  ((pixel&0xf800)>>11) * 8; //r
            p[1] =  ((pixel&(0x07C0))>>6) * 8; //g
            p[2] =  ((pixel&0x001F)>>0) * 8; //b
            p[3] = 0xff;
        }
    }
    return row_ptr;
}
