
#include "parser.h"
#include "zounaclasses.h"

#include <png.h>
#include <string>
#pragma once

class BitmapResult : public ParserResult{
public:
    ~BitmapResult();
    void dump(std::string pathOut) override;
    friend class BitmapParser;
    friend class BitmapBuilder;
    uint32_t getWidth();
    uint32_t getHeight();
    uint8_t getBitmapType();
    std::string inspectSpecific() override;

    void build(std::string pathIn,std::string pathOut) override;
private:
    BitmapResult(std::string pathIn);

    png_bytepp generateRGBA();
    png_bytepp generateRGB();
    png_bytepp generateMonoAlphaRGBFull();

    void freeData(png_bytepp data, int height);
    void writeMonoAlphaRGBFull(FILE *file, png_bytepp rows);
    void writeRGB(FILE *file, png_bytepp rows);
    void writeRGBA(FILE *file, png_bytepp rows);

    uint32_t uCRC1;
    uint32_t uCRC2;
    uint32_t m_width;
    uint32_t m_height;
    uint32_t u1;
    uint8_t m_type;
    uint32_t u2;
    uint16_t *m_pixels;
};

class BitmapParser : public Parser{
public:
    BitmapParser();
    ~BitmapParser();

    BitmapResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
