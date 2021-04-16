
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
    uint32_t getWidth();
    uint32_t getHeight();
    uint8_t getBitmapType();
    std::string inspectSpecific() override;
private:
    BitmapResult(std::string pathIn);

    png_bytepp generateRGBA(FILE *file);
    png_bytepp generateRGB(FILE *file);
    png_bytepp generateMonoAlphaRGBFull(FILE *file);
    uint32_t m_width;
    uint32_t m_height;
    uint8_t m_type;
};

class BitmapParser : public Parser{
public:
    BitmapParser();
    ~BitmapParser();

    BitmapResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
