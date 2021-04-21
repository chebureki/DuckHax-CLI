#include "parser.h"
#include "zounaclasses.h"
#include "parsinghelper.h"

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
    return m_nameCRC;
}

ParserResult *Parser::parseFile(std::string pathIn, CRC32Lookup crcLookup){
    ParserResult *result = new ParserResult(ZounaClasses::UNKNOWN, 0, pathIn);
    return result;
};

std::string ParserResult::inspect(){
    //TODO: fix this madness
    //char buff[128];
    //sprintf(buff,"Path: %s\n\nType: %s\n",m_pathToFile.c_str(),CRC32Lookup::getClassName(m_type));
    std::string inner = formatString("Path: %s\nCRC: %x\nType: %s\n",m_pathToFile.c_str(),m_nameCRC,CRC32Lookup::getClassName(m_type));
    return (INSPECTSEP+inner+INSPECTSEP)+inspectSpecific()+INSPECTSEP;
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

ParserResult::ParserResult(ZounaClasses type, uint32_t nameCRC,std::string pathToFile):
    m_type(type),
    m_nameCRC(nameCRC),
    m_pathToFile(pathToFile){
};
ParserResult::~ParserResult(){};
