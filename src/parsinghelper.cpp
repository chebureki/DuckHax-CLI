#include "parsinghelper.h"
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string>

std::string getFileName(std::string path){
    return path.substr(path.find_last_of("/\\")+1);
}

std::string getFileExtension(std::string path){
    int index = path.find_last_of(".")+1;
    if(index == 0)
        return "";
    else
        return path.substr(index);
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

std::string trailFolder(std::string path){
    if(path.at(path.length()-1)== SEP)
        return path;
    return path+SEP;
}
std::string dontTrailFolder(std::string path){
    int len = path.length();
    if(path.at(len-1)== SEP)
        return path.substr(0,len-1);
    return path;
}

//maybe template and place in the parsinghelper header for future use?
std::string to_hex_string(uint val){
    char chars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string hex = "0x";
    for(int i=7;i>=0;i--){
        hex += chars[(val&(0xf<<(i*4)))>>(i*4)];
    }
    return hex;
}
