#include "parser.h"
#include "zounaclasses.h"

#include <string>
#include <vector>

#pragma once

#define DPCPADDING 0xCD
#define DPCMAGIC "v1.22 - Asobo Studio - Internal Cross Technology"

struct DPCFileData{
    ZounaClasses type;
    uint32_t fileCRC;
    uint32_t offset;
    uint32_t size;
};

typedef std::vector<DPCFileData> DPCFolder;

class DPCResult : public ParserResult{
public:
    ~DPCResult();
    void dump(std::string pathOut) override;
    std::string inspectSpecific() override;

    void build(std::string source, std::string pathOut) override;
    friend class DPCParser;
private:
    DPCResult(std::string pathIn);
    std::vector<DPCFolder> m_folders;
    uint32_t m_blockSize;
    uint32_t u1;
    uint32_t u2;
    uint32_t u3;

    void writeDPCMagic(FILE *file);
    void writePadding(FILE *file,int n);
    void writeHeader(FILE *file);
    void writeFolder(FILE *file, std::string path);
};

class DPCParser : public Parser{
public:
    DPCParser();
    ~DPCParser();

    DPCResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
