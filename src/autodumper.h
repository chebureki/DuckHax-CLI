#include "zounaclasses.h"

#include <string>
#pragma once


ZounaClasses autoDetectFile(std::string pathIn, CRC32Lookup crc);
void autoDump(std::string pathIn, std::string baseNamePathOut, CRC32Lookup crc, bool recursive);
