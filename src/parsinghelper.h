#include <string>

#pragma once

//TODO: order flip function for big endian machines
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define LE(val) val
#endif

#ifdef _WIN32
    #define SEP '\\'
#else
    #define SEP '/'
#endif

std::string getFileName(std::string path);
std::string getFileExtension(std::string path);
std::string getFileBaseName(std::string path); //c:\test.dat => test
std::string getFileBase(std::string path); //c:\test.dat => c:\test

bool isADirectory(std::string path);
