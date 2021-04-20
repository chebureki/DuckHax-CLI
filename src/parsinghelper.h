#include <string>
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

//avoid using these too much!
std::string trailFolder(std::string path);
std::string dontTrailFolder(std::string path);

bool isADirectory(std::string path);

template <typename... Args>
std::string formatString(const std::string format, Args... args){
    int size = std::snprintf(nullptr,0,format.c_str(),args...)+1; // there has got to be a better way
    //char *buff = new char[size];
    char buff[100]; // I am sure this will screw me over later. Is it better than calling new tho?
    std::snprintf(buff,size,format.c_str(),args...);
    std::string result = buff;
    //delete[] buff;
    return result;
}
