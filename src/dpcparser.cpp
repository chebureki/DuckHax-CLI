#include "dpcparser.h"
#include "zounaclasses.h"
#include "parsinghelper.h"

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
        throw std::runtime_error("Couldn't open "+pathIn+" for reading!");
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
    fseek(file,0x800,SEEK_SET); // start of first folder

    while(true){
        uint32_t init = ftell(file);
        int numFiles; //TODO: do sanity checks
        DPCFolder currFolder;
        fread(&numFiles,4,1,file);

        if(numFiles<0 || numFiles > 2000){
            throw std::runtime_error("Invalid number or files: "+std::to_string(numFiles)+", in "+pathIn+"\nOffset is: "+std::to_string(ftell(file))+"\n");
        }

        for(int i=0;i<numFiles;i++){
            DPCFileData dpcFile;
            fread(&dpcFile.size,4,1,file);
            dpcFile.size -= 4;
            dpcFile.offset = ftell(file);
            uint32_t crc;
            fread(&crc,4,1,file);
            fseek(file,-4,SEEK_CUR);
            dpcFile.type = crcLookup.getClass(crc);
            fseek(file,dpcFile.size,SEEK_CUR);

            currFolder.push_back(dpcFile);
        }
        result->m_folders.push_back(currFolder);
        uint32_t end = ftell(file);

        uint32_t predicted = init+((end-init )+(result->m_blockSize-(end-init)));
        if(predicted > totalSize)//done with last folder!
            break;
        fseek(file,predicted,SEEK_SET);
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
            throw std::runtime_error("Can't create "+path+" for writing!\n");
    int folderIndex=1;
    for(auto const &folder : m_folders){
        std::string folderPath = path+std::to_string(folderIndex)+SEP;

        if(!isADirectory(folderPath))
            if ( mkdir(folderPath.c_str(), S_IRWXU)!=0)
                throw std::runtime_error("Can't create "+folderPath+" for writing!\n");
        int fileIndex=1;
        for(auto const dpcFile : folder){
            std::string path = folderPath+std::to_string(fileIndex)+'.'+CRC32Lookup::getClassName(dpcFile.type);
            FILE *file = fopen(path.c_str(),"wb");
            if(file == nullptr)
                 throw std::runtime_error("Can't create "+path+" for writing!\n");
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

std::string DPCResult::inspectSpecific(){
    //TODO: fix this formatting monstrosity!
    std::string output = "Block-size: "+std::to_string(m_blockSize)+ "\n\n";
    output += "Total of "+std::to_string(m_folders.size())+ " folders!\n";
    for (auto const &folder: m_folders){
        output += "\n\tAmount of files: "+ std::to_string(folder.size())+ '\n';
        for(auto const file: folder){
            output+= "\n\t\tType: "+ std::string(CRC32Lookup::getClassName(file.type))+'\n';
            output+= "\t\tSize: "+ std::to_string(file.size)+'\n';
        }
    }
    return output;
}
