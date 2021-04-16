#include "autodumper.h"
#include "bitmapparser.h"
#include "dpcparser.h"
#include "meshparser.h"
#include "sdxparser.h"
#include "soundparser.h"
#include "parsinghelper.h"
#include <dirent.h>
#include <sys/stat.h>
#include <iostream>


ZounaClasses autoDetectFile(std::string pathIn, CRC32Lookup crc){
    FILE *file = fopen(pathIn.c_str(),"rb");
    if(file == nullptr)
        throw std::runtime_error("Can't open "+pathIn+" for reading!\n");
    uint32_t classCRC;
    fread(&classCRC,4,1,file);
    fclose(file);
    file= NULL;
    ZounaClasses zClass = crc.getClass(classCRC);

    //Get the classname from the file-extension, yes I know this is stupid but e.g. SPX and DPC files do not have a CRC for crying out loud
    if(zClass == ZounaClasses::UNKNOWN)
        zClass = crc.getClass(crc.calculateCRC32(getFileExtension(pathIn)));
    return zClass;
}

Parser *getMatchingParser(std::string pathIn, CRC32Lookup crc){
    ZounaClasses type;
    type = autoDetectFile(pathIn,crc);

    //TODO: avoid having to initialize a parser object every darn time
    Parser *parser;
    switch(type){

    case ZounaClasses::Bitmap_Z:
        parser = new BitmapParser;
        break;
    case ZounaClasses::Mesh_Z:
        parser = new MeshParser;
        break;

    case ZounaClasses::Sound_Z:
        parser = new SoundParser;
        break;
    case ZounaClasses::sdx:{
        parser = new SdxParser;
        break;
    }

    case ZounaClasses::DPC:
        parser = new DPCParser;
    default:
        //Unknown file
        return nullptr;
    }
    return parser;
}

//TODO: fix this garbage code. ffs I can't be this bad at c++ right?
void autoDump(std::string pathIn, std::string baseNamePathOut, CRC32Lookup crc, bool recursive){
    if(isADirectory(pathIn)){
        struct dirent *entry = nullptr;
        DIR *dir = nullptr;
        dir = opendir(pathIn.c_str());
        while( (entry=readdir(dir))){
            std::string childPath = entry->d_name;
            //delete entry;
            if (childPath=="." || childPath == "..")
                continue;
            std::string totalPath = trailFolder(pathIn)+childPath;
            if(isADirectory(totalPath)&&recursive){
                if(!isADirectory(baseNamePathOut))
                    mkdir(baseNamePathOut.c_str(),S_IRWXU);
                std::string newbase= trailFolder(baseNamePathOut)+childPath;
                if (!isADirectory(newbase))
                    if ( mkdir(newbase.c_str(), 0700) != 0)
                        throw std::runtime_error("Can't create "+newbase+" for writing!\n");
                autoDump(trailFolder(totalPath),trailFolder(newbase),crc,recursive);
                continue;
            }
            autoDump(totalPath,getFileBase(baseNamePathOut+childPath),crc,recursive);
        }
        closedir(dir);
        return;
    }
    ZounaClasses type;
    try{
        type = autoDetectFile(pathIn,crc);
    }catch(std::runtime_error e){
        std::cout<<e.what();
        return;
    }
    //TODO: avoid having to initialize a parser object every darn time
    Parser *parser;
    std::string fileExt;
    switch(type){

    case ZounaClasses::Bitmap_Z:
        parser = new BitmapParser;
        fileExt="png";
        break;
    case ZounaClasses::Mesh_Z:
        parser = new MeshParser;
        fileExt="obj";
        break;

    case ZounaClasses::Sound_Z:
        parser = new SoundParser;
        fileExt="wav";
        break;
    case ZounaClasses::sdx:{
        parser = new SdxParser;
        ParserResult *result = parser->parseFile(pathIn,crc);
        result->dump(trailFolder(baseNamePathOut));
        delete parser;
        delete result;
        return;
    }

    case ZounaClasses::DPC:
        parser = new DPCParser;
        try{
            ParserResult *result = parser->parseFile(pathIn,crc);
            std::string path = trailFolder(baseNamePathOut);
            result->dump(path);
            autoDump(path,path,crc,true);
            delete parser;
            delete result;
        }catch(std::runtime_error e){
            std::cout<<e.what();
        }
        return;
    default:
        //Unknown file
        return;
    }
    ParserResult *result;
    result = parser->parseFile(pathIn,crc);
    if (result == nullptr)
        throw std::runtime_error("Failed to parse "+pathIn);
    result->dump(baseNamePathOut+"."+fileExt);
    delete parser;
    delete result;
}
