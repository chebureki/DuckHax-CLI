#include "parser.h"
#include <stdio.h>
#include <iostream>

uint32_t ParserResult::getType()const {
    return m_type;
}

uint32_t ParserResult::getCRC32()const {
    return m_crc32;
}

ParserResult *Parser::parseFile(std::string pathIn, CRC32Lookup crcLookup){
    ParserResult *result = new ParserResult(ZounaClasses::UNKNOWN, pathIn);
    return result;
};

Parser::Parser(){};
Parser::~Parser(){};

void ParserResult::dump(std::string pathOut){
    throw std::logic_error("You shouldn't dump an meta-class, you dingus!");
}
std::string ParserResult::getFilePath() const{
    return m_pathToFile;
};

ParserResult::ParserResult(ZounaClasses type, std::string pathToFile): m_type(type), m_pathToFile(pathToFile){};
ParserResult::~ParserResult(){};
