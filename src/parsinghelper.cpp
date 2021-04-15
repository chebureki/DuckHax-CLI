#include "parsinghelper.h"
#include <limits.h>
#include <stdlib.h>
#include <iostream>
#include <sys/stat.h>

std::string getFileName(std::string path){
    return path.substr(path.find_last_of("/\\")+1);
}

std::string getFileExtension(std::string path){
    return path.substr(path.find_last_of(".")+1);
}

std::string getFileBase(std::string path){
    return path.substr(0,path.find_last_of("."));
}
std::string getFileBaseName(std::string path){
    std::string fileName = getFileName(path);
    return fileName.substr(0,fileName.find_last_of("."));
}

bool isADirectory(std::string path){
    //strip trailing slash
    if(path.at(path.length()-1)== SEP)
        path = path.substr(0,path.length()-1);
    struct stat s;
    if(stat(path.c_str(),&s)==0){
        if(s.st_mode&S_IFDIR){
            return true;
        }
    }

    return false;
}