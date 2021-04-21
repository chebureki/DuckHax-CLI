#include "zounaclasses.h"

#include <string>
#include <stdint.h>

#pragma once
#define INSPECTSEP "--------------------\n"

class ParserResult{
    private:
        ZounaClasses m_type;
        uint32_t m_nameCRC;
        std::string m_pathToFile;
    public:
        friend class Parser;
        ParserResult(ZounaClasses type, uint32_t nameCRC, std::string pathToFile);
        virtual ~ParserResult();
        uint32_t getType() const;
        uint32_t getCRC32() const;
        virtual std::string inspectSpecific(); // returns file-path, file-type, ... and so forth
        std::string inspect(); // returns file-path, file-type, ... and so forth, followed by file-specifics
        std::string getFilePath() const;
        virtual void dump(std::string pathOut);

        virtual void build(std::string pathIn, std::string pathOut);
};


class Parser{
    public:
        Parser();
        virtual ~Parser();

        virtual ParserResult *parseFile(std::string pathIn, CRC32Lookup crcLookup);
};
