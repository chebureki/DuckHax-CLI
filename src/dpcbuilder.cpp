#include "dpcparser.h"
#include "parsinghelper.h"
#include "errormessages.h"

#include <stdio.h>
#include <dirent.h>
#include <stdexcept>

void DPCResult::writeHeader(FILE *file){
    fwrite(&m_blockSize,4,1,file);
    fwrite(&u1,4,1,file);
    fwrite(&u2,4,1,file);
    fwrite(&u3,4,1,file);
    writePadding(file,0x800-ftell(file));
}

void DPCResult::writeDPCMagic(FILE *file){
    char magic[] = DPCMAGIC;
    fwrite(magic,1,sizeof (magic),file);
    writePadding(file,0x100-sizeof(DPCMAGIC));
}

void DPCResult::writePadding(FILE *file,int n){
    //TODO: make writing padding more efficient
    uint8_t padding = 0xCD;
    for(int i=0;i<n;i++)
    fwrite(&padding,1,1,file);
}

//TODO: FIX THIS GARBAGE CODE FFS
void DPCResult::writeFolder(FILE *file, std::string path){
    int countPos = ftell(file);
    int end = 0;
    int count = 0;//number is **actually** set AFTER writing all the files
    fwrite(&count,4,1,file);
    DIR *dir = nullptr;
    uint8_t buff[5000000];
    struct dirent *entry = nullptr;
    dir = opendir(path.c_str());

    if (dir == nullptr)
        throw CANT_READ(path.c_str());
    while( (entry=readdir(dir))){
        std::string childPath = entry->d_name;
        if (childPath=="." || childPath == "..")
            continue;
        //TODO: error checking!
        FILE *child = fopen((trailFolder(path)+childPath).c_str(),"rb");
        fseek(child,0,SEEK_END);
        uint32_t size = ftell(child);
        fseek(child,0,SEEK_SET);
        fread(buff,1,size,child);
        fwrite(buff,1,size,file);

        fclose(child);
        end = ftell(file);
        count++;
    }
    closedir(dir);
    fseek(file,countPos,SEEK_SET);
    fwrite(&count,4,1,file);
    fseek(file,end,SEEK_SET);
}

void DPCResult::build(std::string source, std::string pathOut){
    if(!isADirectory(source))
        throw NOT_A_DIR(source.c_str());
    FILE *file = fopen(pathOut.c_str(),"wb");
    if(file == nullptr)
        throw CANT_WRITE(pathOut.c_str());
    writeDPCMagic(file);
    writeHeader(file);

    DIR *dir = nullptr;
    struct dirent *entry = nullptr;
    dir = opendir(source.c_str());
    while( (entry=readdir(dir))){
        std::string childPath = entry->d_name;
        if (childPath=="." || childPath == "..")
            continue;
        uint32_t pre = ftell(file);
        writeFolder(file,trailFolder(source)+childPath);
        uint32_t post = ftell(file);
        writePadding(file, m_blockSize-(post-pre));
    }
    closedir(dir);
    fclose(file);
}
