#include "parser.h"
#include "zounaclasses.h"

#include <stdio.h>
#include <iostream>
#include <string.h>

void ParserResult::build(std::string pathIn, std::string pathOut){
    std::cerr<<"Building for "<< CRC32Lookup::getClassName(m_type) << " is not supported, yet!\n";
}
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

std::string ParserResult::inspect(){
    //TODO: fix this madness
    std::string output = INSPECTSEP;
    char buff[128];
    sprintf(buff,"Path: %s\nType: %s\n",m_pathToFile.c_str(),CRC32Lookup::getClassName(m_type));
    return (output+buff+INSPECTSEP)+inspectSpecific()+INSPECTSEP;
}

Parser::Parser(){};
Parser::~Parser(){};

void ParserResult::dump(std::string pathOut){
    throw std::logic_error("You shouldn't dump an meta-class, you dingus!");
}

std::string ParserResult::inspectSpecific(){
    return "Can't decipher file, sorry :/\n";
}

std::string ParserResult::getFilePath() const{
    return m_pathToFile;
};

ParserResult::ParserResult(ZounaClasses type, std::string pathToFile): m_type(type), m_pathToFile(pathToFile){
    m_crc32 = CRC32Lookup::calculateCRC32(CRC32Lookup::getClassName(type)); //TODO: fix this waste of computing power!
};
ParserResult::~ParserResult(){};
