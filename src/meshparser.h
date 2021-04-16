
#include "parser.h"
#include "zounaclasses.h"

#include <string>
#include <vector>
#pragma once

struct TriangleStrip{
    uint32_t u7;
    uint32_t u8;
    uint32_t countIndice;

    std::vector<uint16_t> indices;
};

struct Vertex3f{
    float x;
    float y;
    float z;
};

class MeshResult : public ParserResult{
public:
    ~MeshResult();
    void dump(std::string pathOut) override;
    std::string inspectSpecific() override;
    friend class MeshParser;
    uint32_t getVerticeCount();
    uint32_t getFaceCount();
private:
    MeshResult(std::string pathIn);
    uint32_t m_countVertice;
    uint32_t m_countFaces;
    uint32_t m_countStrips;
    std::vector<Vertex3f> m_vertices;
    std::vector<TriangleStrip> m_strips;
};

class MeshParser : public Parser{
public:
    MeshParser();
    ~MeshParser();

    MeshResult *parseFile(std::string pathIn, CRC32Lookup crcLookup) override;
private:
};
