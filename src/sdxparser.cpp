#include "sdxparser.h"
#include "soundparser.h"
#include "parsinghelper.h"
#include "errormessages.h"

#include <stdio.h>
#include <stdexcept>
#include <string.h>
#include <sys/stat.h>

#include <iostream>


SdxParser::SdxParser(){}
SdxParser::~SdxParser(){}

SdxResult *SdxParser::parseFile(std::string pathIn, CRC32Lookup crcLookup){
    FILE *file = fopen(pathIn.c_str(),"rb");
    if(file == nullptr)
        throw CANT_READ(pathIn.c_str());
    SdxResult *result = new SdxResult(pathIn);
    while(1){
        char line[16];
        fread(line,1,16,file);
        if ( strncmp(line,"TT ",3) != 0)
            break;

        SdxResult::SdxDialog dialog;
        //char name[16]; // = line+3;
        //strncpy(name,line+3,strlen(line+3));
        dialog.name = std::string(line+3,strlen(line+3));
        fseek(file,4,SEEK_CUR); //possibly the frequency
        uint32_t offset;
        fread(&offset, 4,1,file);
        fread(&dialog.size, 4,1,file);

        uint32_t curr = ftell(file);
        fseek(file,0x800+offset,SEEK_SET);
        dialog.packetBuff = new uint8_t[dialog.size];
        fread(dialog.packetBuff,1,dialog.size,file);
        fseek(file,curr,SEEK_SET);

        fseek(file,4,SEEK_CUR);
        result->m_dialogs.push_back(dialog);
    }
    fclose(file);
    return result;
}

void SdxResult::dump(std::string pathOut){
    if(!isADirectory(pathOut))
        if ( mkdir(pathOut.c_str(), S_IRWXU)!=0)
            throw CANT_WRITE(pathOut.c_str());

    for(const SdxDialog &dialog : m_dialogs){
        std::string path = pathOut+dialog.name;
        SoundResult sound(path,0);
        sound.m_frameRate = 22050;
        sound.m_countPackets = dialog.size;
        sound.m_packetBuff = dialog.packetBuff;

        sound.dump(path);
    }
    return;
}

SdxResult::SdxResult(std::string pathIn): ParserResult(ZounaClasses::sdx,0,pathIn){};
SdxResult::~SdxResult(){
    //packetbuff is deallocated by soundresult!
    //TODO: fix this behavior, what if we would want to dump multiple times?
    m_dialogs.clear();
}

std::string SdxResult::inspectSpecific(){
    //TODO: fix this formatting monstrosity!
    std::string output = "Total of "+std::to_string(m_dialogs.size())+ " dialogs!\n";
    for (auto const &dialog: m_dialogs){
        output += "\n\tName: "+ dialog.name+ '\n';
        output += "\tSize: "+ std::to_string(dialog.size)+ '\n';
    }
    return output;
}
