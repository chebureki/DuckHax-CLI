#include "soundparser.h"

#include <string>
#include <stdio.h>
#include <stdexcept>

//Stolen from https://gist.github.com/csukuangfj/c1d1d769606260d436f8674c30662450
struct WaveHeader {
    /* RIFF Chunk Descriptor */
    uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
    uint32_t ChunkSize;                     // RIFF Chunk Size
    uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
    /* "fmt" sub-chunk */
    uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
    uint32_t Subchunk1Size = 16;           // Size of the fmt chunk
    uint16_t AudioFormat = 1; // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
                                // Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t NumOfChan = 1;   // Number of channels 1=Mono 2=Sterio
    uint32_t SamplesPerSec = 16000;   // Sampling Frequency in Hz
    uint32_t bytesPerSec = 16000 * 2; // bytes per second
    uint16_t blockAlign = 2;          // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample = 16;      // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
    uint32_t Subchunk2Size;                        // Sampled data length
};

SoundParser::SoundParser(){}
SoundParser::~SoundParser(){}

void SoundResult::writeWaveHeader(FILE *file){
    WaveHeader header;
    //these parameters may need some work, I am not entirely sure
    //some files are fine, some are not
    header.ChunkSize = m_countPackets + sizeof(WaveHeader)-8;
    header.Subchunk2Size = m_countPackets + sizeof(WaveHeader) - 44;
    header.SamplesPerSec = m_frameRate;

    fwrite(&header,1,sizeof (WaveHeader),file);
}
SoundResult *SoundParser::parseFile(std::string pathIn, CRC32Lookup crcLookup){
    FILE *file = fopen(pathIn.c_str(),"rb");
    if(file == nullptr)
        throw std::runtime_error("Can't open "+pathIn+" for reading!");
    SoundResult *result = new SoundResult(pathIn);

    uint32_t fileMagic;
    fread(&fileMagic,4,1,file);
    if(crcLookup.getClass(fileMagic) != ZounaClasses::Sound_Z){
        fclose(file);
        delete result;
        throw std::runtime_error(pathIn+" is not a soundfile!");
    }
    fseek(file,0x10,SEEK_SET);
    fread(&result->m_frameRate,4,1,file);

    fseek(file,0x18,SEEK_SET);
    fread(&result->m_countPackets,4,1,file);

    result->m_packetBuff = new uint8_t[result->m_countPackets];
    fread(result->m_packetBuff,1,result->m_countPackets,file);
    fclose(file);
    return result;
}

SoundResult::SoundResult(std::string pathIn): ParserResult(ZounaClasses::Sound_Z,pathIn){};
SoundResult::~SoundResult(){
    delete[] m_packetBuff;
}

void SoundResult::dump(std::string pathOut){
    FILE *file = fopen(pathOut.c_str(),"wb");
    if (file == nullptr)
        throw std::runtime_error("Can't open "+pathOut+" for writing!");
    writeWaveHeader(file);
    fwrite(m_packetBuff,1,m_countPackets,file);
    fclose(file);
    return;
}
