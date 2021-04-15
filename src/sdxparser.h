#pragma once

#include "parser.h"
#include "zounaclasses.h"

#include <string>
#include <vector>
#include <stdint.h>
#pragma once

class SdxResult : public ParserResult{
public:
    ~SdxResult();
    void dump(std::string pathOut) override;
    friend class SdxParser;

private:
    SdxResult(std::string pathIn);
    struct SdxDialog{
        std::string name;
        uint32_t size;
        uint8_t *packetBuff;
    };
    std::vector<SdxDialog> m_dialogs;
};

class SdxParser : public Parser{
public:
    SdxParser();
    ~SdxParser();

    SdxResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
