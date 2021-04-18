#include "bitmapparser.h"

#include <png.h>
#include <iostream>
#include <stdio.h>
#include <stdexcept>

//Shamelessly copied parts from https://github.com/abzico/libpng_example/blob/master/test-libpng.c
png_bytepp readPngFile(std::string path, int *rowBytes, int *width, int*height){
    unsigned char header[8];
    FILE *file = fopen(path.c_str(),"rb");
    if (file == nullptr)
        throw std::runtime_error("Couldn't open "+path+" for reading!\n");

    const int cmpNum = 8;
    if( fread(header,1,cmpNum,file) != cmpNum){
        fclose(file);
        throw std::runtime_error("Error in reading "+path+": png is wayyy to small lmao!\n");
    }
    if (png_sig_cmp(header,0,cmpNum) != 0){
        fclose(file);
        throw std::runtime_error("Error in reading "+path+": this is not a valid png!\n");
    }

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
    if (png == NULL){
        fclose(file);
        throw std::runtime_error("Error in libpng for "+path+"!\n");
    }

    png_infop pngInfo = png_create_info_struct(png);
    if (pngInfo == nullptr){
        fclose(file);
        png_destroy_read_struct(&png, NULL ,NULL);
        throw std::runtime_error("Error in libpng for "+path+"!\n");
    }

    if (setjmp(png_jmpbuf(png))){
        fclose(file);
        png_destroy_read_struct(&png, NULL ,NULL);
        throw std::runtime_error("Error in libpng for "+path+"!\n");
    }

    png_init_io(png,file);
    png_set_sig_bytes(png, cmpNum);
    png_read_info(png,pngInfo);
    if(width != nullptr)
        *width = png_get_image_width(png, pngInfo);
    if (height != nullptr)
        *height = png_get_image_height(png, pngInfo);
    if (rowBytes != nullptr)
        *rowBytes = png_get_rowbytes(png, pngInfo);

    png_bytepp rows = (png_bytepp)malloc(sizeof(png_bytep) * *height);
    for(int y=0;y< *height;y++)
        rows[y] = (png_bytep)malloc(*rowBytes);
    png_read_image(png,rows);
    png_destroy_read_struct(&png, &pngInfo, NULL);

    fclose(file);
    return rows;
}

void BitmapResult::build(std::string source, std::string pathOut){
    FILE *file = fopen(pathOut.c_str(),"wb");
    if(file == nullptr)
        throw std::runtime_error("Couldn't open "+pathOut+" for writing!\n");

    int fileSize = 4+4+4+4+4+4+4+1+4+1+(2*(m_height * m_width))+4;
    fwrite(&fileSize,4,1,file);
    uint32_t crc = getCRC32(); //TODO: this is probably set nowhere
    fwrite(&crc,4,1,file);
    fwrite(&uCRC1,4,1,file);
    fwrite(&uCRC2,4,1,file);

    fwrite(&m_width,4,1,file);
    fwrite(&m_height,4,1,file);
    fwrite(&u1,4,1,file);
    fwrite(&m_type,1,1,file);
    fwrite(&u2,4,1,file);

    /*
     * TODO:
     * Actually read a png lol
     */
    int rowBytes;
    int height;
    int width;
    png_bytepp data = readPngFile(source,&rowBytes,&width,&height);
    if (height != m_height || width != m_width)
        throw std::runtime_error("As of now: the source and reference image should have the same dimensions!\n");
    std::cout << width << 'x' << height<<" row-bytes: "<<rowBytes<<std::endl;
    //TODO: create a bitmaptype enum
    switch (m_type) {
    case 7:
        writeMonoAlphaRGBFull(file, data);
        break;
    case 8:
        writeRGB(file,data);
        break;
    case 0xA:
        writeRGBA(file,data);
        break;
    default:
        fclose(file);
        throw std::runtime_error("Type "+ std::to_string(m_type)+" is not supported for building yet!\n");
        break;
    }


    uint8_t startIndicator = 0x15;
    fwrite(&startIndicator,1,1,file);
    uint32_t zero = 0;
    fwrite(&zero,4,1,file);
    fclose(file);
}

void BitmapResult::writeMonoAlphaRGBFull(FILE *file, png_bytepp rows){
    for(uint32_t y=0;y<m_height;y++){
        png_bytep row = rows[y];
        for(uint32_t x=0;x<m_width;x++){
            png_bytep p = &row[x*4];
            uint16_t pixel = 0;
            //What the fuck! Why is the endianness flipped? Honestly, just shoot me at this point
            pixel |= ((uint16_t)(p[0]/8)) <<10; //r
            pixel |= ((uint16_t)(p[1]/8)) <<5; //g
            pixel |= ((uint16_t)(p[2]/8)) <<0; //b
            pixel |= ((uint16_t)(p[3]/255)) <<15; //a
            pixel = ((pixel&0xff00)>>8)|((pixel&0x00ff)<<8); //flip endianness
            fwrite(&pixel,2,1,file);
        }
    }
}

void BitmapResult::writeRGB(FILE *file, png_bytepp rows){
    for(uint32_t y=0;y<m_height;y++){
        png_bytep row = rows[y];
        for(uint32_t x=0;x<m_width;x++){
            png_bytep p = &row[x*4];
            uint16_t pixel = 0;
            //What the fuck! Why is the endianness flipped? Honestly, just shoot me at this point
            pixel |= ((uint16_t)(p[0]/8)) <<11; //r
            pixel |= ((uint16_t)(p[1]/8)) <<6; //g
            pixel |= ((uint16_t)(p[2]/8)) <<0; //b
            //there is no alpha value, duh
            pixel = ((pixel&0xff00)>>8)|((pixel&0x00ff)<<8); //flip endianness
            fwrite(&pixel,2,1,file);
        }
    }
}

void BitmapResult::writeRGBA(FILE *file, png_bytepp rows){
    for(uint32_t y=0;y<m_height;y++){
        png_bytep row = rows[y];
        for(uint32_t x=0;x<m_width;x++){
            png_bytep p = &row[x*4];
            uint16_t pixel = 0;
            //What the fuck! Why is the endianness flipped? Honestly, just shoot me at this point
            pixel |= ((uint16_t)(p[0]/16)) <<8; //r
            pixel |= ((uint16_t)(p[1]/16)) <<4; //g
            pixel |= ((uint16_t)(p[2]/16)) <<0; //b
            pixel |= ((uint16_t)(p[3]/16)) <<12; //a
            pixel = ((pixel&0xff00)>>8)|((pixel&0x00ff)<<8); //flip endianness
            fwrite(&pixel,2,1,file);
        }
    }
}
