#include "dpcparser.h"
#include "zounaclasses.h"
#include "parsinghelper.h"
#include "errormessages.h"

#include <stdio.h>
#include <string.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include <stdexcept>

DPCResult *DPCParser::parseFile(std::string pathIn,CRC32Lookup crcLookup){
    DPCResult *result = new DPCResult(pathIn);
    FILE *file;
    file = fopen(pathIn.c_str(),"rb");
    if (file == nullptr){
        throw CANT_READ(pathIn.c_str());
    }

    char fileMagic[49];
    fread(&fileMagic,49,1,file);
    //TODO: better fileMagic checking
    if (strcmp(fileMagic,"v1.22 - Asobo Studio - Internal Cross Technology")!=0){
        if(strcmp(fileMagic, "v1.17 - Asobo Studio - Internal Cross Technology") != 0)
            if(strcmp(fileMagic, "v1.14 - Asobo Studio - Internal Cross Technology") != 0)
                throw std::invalid_argument(pathIn+ " is either outdated/too new or not a DPC-file");
    }
    fseek(file,0,SEEK_END);
    uint32_t totalSize = ftell(file);
    fseek(file,0x100,SEEK_SET);
    fread(&result->m_blockSize,4,1,file);
    fread(&result->u1,4,1,file);
    fread(&result->u2,4,1,file);
    fread(&result->u3,4,1,file);
    fseek(file,0x800,SEEK_SET); // start of first folder

    for(int i=0;i<(int)(totalSize/result->m_blockSize);i++){
        int numFiles; //TODO: do sanity checks
        DPCFolder currFolder;
        fread(&numFiles,4,1,file);

        if(numFiles<0 || numFiles > 2000){
            throw std::runtime_error("Invalid number or files: "+std::to_string(numFiles)+", in "+pathIn+"\nOffset is: "+std::to_string(ftell(file))+"\n");
        }

        uint32_t folderSize = 0;
        for(int i=0;i<numFiles;i++){
            DPCFileData dpcFile;

            dpcFile.offset = ftell(file);
            fread(&dpcFile.size,4,1,file);
            folderSize+=dpcFile.size;

            uint32_t crc;
            fread(&crc,4,1,file);
            dpcFile.type = crcLookup.getClass(crc);

            fread(&dpcFile.fileCRC,4,1,file);
            fseek(file,-12,SEEK_CUR);
            fseek(file,dpcFile.size,SEEK_CUR);
            currFolder.push_back(dpcFile);
        }
        result->m_folders.push_back(currFolder);
        fseek(file, result->m_blockSize-folderSize-4,SEEK_CUR);
    }
    fclose(file);
    return result;
}

void DPCResult::dump(std::string pathOut){
    std::string inputFilePath = getFilePath();
    FILE *inputFile = fopen(inputFilePath.c_str(),"rb");
    fseek(inputFile,0,SEEK_END);
    uint8_t fileBuff[5000000]; //5MB should suffice
    fseek(inputFile,0,SEEK_SET);

    std::string path = pathOut;
    if(!isADirectory(path))
        if ( mkdir(path.c_str(), S_IRWXU)!=0)
            throw CANT_MKDIR(path.c_str());
    int folderIndex=1;
    for(auto const &folder : m_folders){
        std::string folderPath = path+std::to_string(folderIndex)+SEP;

        if(!isADirectory(folderPath))
            if ( mkdir(folderPath.c_str(), S_IRWXU)!=0)
                throw CANT_MKDIR(folderPath.c_str());
        int fileIndex=1;
        for(auto const dpcFile : folder){
            std::string path = folderPath+std::to_string(fileIndex)+'.'+CRC32Lookup::getClassName(dpcFile.type);
            FILE *file = fopen(path.c_str(),"wb");
            if(file == nullptr)
                throw CANT_WRITE(path.c_str());
            fseek(inputFile,dpcFile.offset,SEEK_SET);
            fread(&fileBuff,1,dpcFile.size,inputFile);
            fwrite(&fileBuff,1,dpcFile.size,file);
            fclose(file);
            fileIndex++;
        }
        folderIndex++;
    }
    fclose(inputFile);
}

DPCParser::DPCParser(){};
DPCParser::~DPCParser(){
};
DPCResult::DPCResult(std::string pathIn): ParserResult(ZounaClasses::DPC,pathIn){};
DPCResult::~DPCResult(){
    for(auto folder: m_folders)
        folder.clear();
    m_folders.clear();
};

//maybe template and place in the parsinghelper header for future use?
std::string to_hex_string(uint val){
    char chars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    std::string hex = "0x";
    for(int i=7;i>=0;i--){
        hex += chars[(val&(0xf<<(i*4)))>>(i*4)];
    }
    return hex;
}

std::string DPCResult::inspectSpecific(){
    //TODO: fix this formatting monstrosity!
    std::string output = "Block-size: "+std::to_string(m_blockSize)+ "\n\n";
    output += "Total of "+std::to_string(m_folders.size())+ " folders!\n";
    for (auto const &folder: m_folders){
        output += "\n\tAmount of files: "+ std::to_string(folder.size())+ '\n';
        for(auto const file: folder){
            output+= "\n\t\tType: "+ std::string(CRC32Lookup::getClassName(file.type))+'\n';
            output+= "\t\tCRC: "+ to_hex_string(file.fileCRC)+'\n';
            output+= "\t\tSize: "+ std::to_string(file.size)+'\n';
        }
    }
    return output;
}
