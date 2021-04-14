#include "zounaclasses.h"

#include <string>
#include <stdint.h>

#pragma once
class ParserResult{
    private:
        ZounaClasses m_type;
        uint32_t m_crc32;
        std::string m_pathToFile;
    public:
        friend class Parser;
        ParserResult(ZounaClasses type, std::string pathToFile);
        virtual ~ParserResult();
        uint32_t getType() const;
        uint32_t getCRC32() const;
        std::string getFilePath() const;
        virtual void dump(std::string pathOut);
};


class Parser{
    public:
        Parser();
        virtual ~Parser();

        virtual ParserResult *parseFile(std::string pathIn, CRC32Lookup crcLookup);
};
