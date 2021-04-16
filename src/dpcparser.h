#include "parser.h"
#include "zounaclasses.h"

#include <string>
#include <vector>

#pragma once
struct DPCFileData{
    ZounaClasses type;
    uint32_t offset;
    uint32_t size;
};

typedef std::vector<DPCFileData> DPCFolder;

class DPCResult : public ParserResult{
public:
    ~DPCResult();
    void dump(std::string pathOut) override;
    std::string inspectSpecific() override;
    friend class DPCParser;
private:
    DPCResult(std::string pathIn);
    std::vector<DPCFolder> m_folders;
};

class DPCParser : public Parser{
public:
    DPCParser();
    ~DPCParser();

    DPCResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
