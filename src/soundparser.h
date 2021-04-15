#pragma once

#include "parser.h"
#include "zounaclasses.h"

#include <string>
#include <stdint.h>
#pragma once

class SoundResult : public ParserResult{
public:
    ~SoundResult();
    void dump(std::string pathOut) override;
    friend class SoundParser;

private:
    void writeWaveHeader(FILE *file);
    SoundResult(std::string pathIn);
    uint32_t m_countPackets;
    uint32_t m_countChannels; //TODO: is this really necessary? Aren't all soundfiles monochaneled?
    uint32_t m_frameRate;

    uint8_t *m_packetBuff;
};

class SoundParser : public Parser{
public:
    SoundParser();
    ~SoundParser();

    SoundResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
