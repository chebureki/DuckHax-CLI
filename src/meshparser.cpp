#include "meshparser.h"
#include "errormessages.h"

#include <stdio.h>
#include <stdexcept>
#include <iostream>

MeshParser::MeshParser(){}
MeshParser::~MeshParser(){}

MeshResult::MeshResult(std::string pathIn, uint32_t nameCRC): ParserResult(ZounaClasses::Mesh_Z, nameCRC,pathIn){};
MeshResult::~MeshResult(){};

std::string objTriangleStrip(std::vector<TriangleStrip> strips){
    std::string faces = "";
    for (auto const strip: strips){
        for(int i=0;i<(int)strip.countIndice;i++){
            if( i>0 && i<(int)(strip.countIndice-1))
                faces += "f " + std::to_string(strip.indices[i]+1)+" "+ std::to_string(strip.indices[i-1]+1)+" "+ std::to_string(strip.indices[i+1]+1)+'\n';
            if (i<(int)strip.countIndice-2)
                faces += "f " + std::to_string(strip.indices[i]+1)+" "+ std::to_string(strip.indices[i+1]+1)+" "+ std::to_string(strip.indices[i+2]+1)+'\n';
        }
    }
    return faces;
}

std::string objVertice(std::vector<Vertex3f> vertice){
    std::string vdecl = "";
    for(auto const vertex: vertice){
        vdecl+= "v "+ std::to_string(vertex.x)+" "+std::to_string(vertex.y)+" "+std::to_string(vertex.z) +'\n';
    }
    return vdecl;
}

void MeshResult::dump(std::string pathOut){
    FILE *file = fopen(pathOut.c_str(),"wb");
    if(file == nullptr)
        throw CANT_WRITE(pathOut.c_str());
    std::string obj = "";
    obj+=objVertice(m_vertices);
    obj+=objTriangleStrip(m_strips);
    const char *raw = obj.c_str();
    fwrite(raw,1,obj.length(),file);
    fclose(file);
}

MeshResult *MeshParser::parseFile(std::string pathIn, CRC32Lookup crcLookup){
    FILE *file = fopen(pathIn.c_str(),"rb");
    if(file == nullptr){
        throw CANT_READ(pathIn.c_str());
    }
    fseek(file,4,SEEK_SET); //ignore the file-size!
    uint32_t fileMagic;
    fread(&fileMagic,4,1,file);
    if(crcLookup.getClass(fileMagic) != ZounaClasses::Mesh_Z)
        throw std::invalid_argument(pathIn+" is not a mesh file!");

    uint32_t crc;
    fread(&crc,4,1,file);
    MeshResult *result = new MeshResult(pathIn,crc);
    //Please ignore this p.o.s. code, I do not know all the variables myself
    //TODO: create a reading macro so declaration and reading are in one line
    fseek(file,96,SEEK_SET);
    fread(&result->m_countVertice,4,1,file);

    for(int i=0;i<(int)result->m_countVertice;i++){
        Vertex3f vertex;
        fread(&vertex.x,4,1,file);
        fread(&vertex.y,4,1,file);
        fread(&vertex.z,4,1,file);
        result->m_vertices.push_back(vertex);
    }

    uint32_t uCount1;
    fread(&uCount1,4,1,file);
    fseek(file,uCount1*8, SEEK_CUR);
    uint32_t uCount2;
    fread(&uCount2,4,1,file);
    fseek(file,uCount2*12,SEEK_CUR);

    fread(&result->m_countStrips,4,1,file);
    uint32_t uCount3;
    fread(&uCount3,4,1,file);
    result->m_countFaces = (uCount3*2)-4;

    //no idea why the first strip is a special case
    TriangleStrip initial;
    initial.u7 =32;
    initial.u8 = 2; //just a guess
    initial.countIndice = uCount3;
    for(int i=0;i<(int)initial.countIndice;i++){
        uint16_t index;
        fread(&index,2,1,file);
        initial.indices.push_back(index);

    }
    result->m_strips.push_back(initial);
    for(int i=1;i<(int)result->m_countStrips;i++){
        TriangleStrip strip;
        fread(&strip.u7,4,1,file);
        fread(&strip.u8,4,1,file);
        fread(&strip.countIndice,4,1,file);
        result->m_countFaces += (strip.countIndice*2)-4;

        for(int j=0;j<(int)strip.countIndice;j++){
            uint16_t index;
            fread(&index,2,1,file);
            strip.indices.push_back(index);
        }
        result->m_strips.push_back(strip);
    }
    fclose(file);
    return result;
}

uint32_t MeshResult::getFaceCount(){
    return m_countFaces;
}

uint32_t MeshResult::getVerticeCount(){
    return m_countVertice;
}

std::string MeshResult::inspectSpecific(){
    //TODO: fix this formatting monstrosity!
    //std::string output = "Total of "+std::to_string(m_dialogs.size())+ " dialogs!\n";
    std::string output = "";
    output += "Amount of Vertices: " + std::to_string(m_countVertice) +'\n';
    output += "Amount of Faces: " + std::to_string(m_countFaces) +'\n';
    return output;
}
