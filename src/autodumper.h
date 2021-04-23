#include "zounaclasses.h"
#include "parser.h"

#include <string>
#pragma once


ZounaClasses autoDetectFile(std::string pathIn, CRC32Lookup crc);
void autoDump(std::string pathIn, std::string baseNamePathOut, CRC32Lookup crc, bool recursive, bool autoConv);
Parser *getMatchingParser(ZounaClasses type, CRC32Lookup crc);
