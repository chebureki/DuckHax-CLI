#include "autodumper.h"
#include "parsinghelper.h"

#include <iostream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <string.h>

void printHelp(){
    std::cout<<"USAGE: DuckHax [MODE] -i INPUT -o OUTPUT\nMODES: dump | inspect \n";
}

int main(int argc, char **argv)
{
    if (argc < 2){
        printHelp();
        exit(1);
    }

    int c;
    char *pathIn = nullptr;
    char *pathOut = nullptr;
    while((c = getopt(argc-1 , argv+1,"i:o:")) != -1){
        switch(c){
        case 'i':
            pathIn = optarg;
            break;
        case 'o':
            pathOut = optarg;
            break;
        case '?':
            printHelp();
            exit(1);
            break;
        }
    }
    char *mode = argv[1];
    CRC32Lookup crc{};
    if (strcmp(mode, "dump")==0){

        if (pathOut == nullptr || pathIn == nullptr){
            std::cerr <<"Specify input and output!\n";
            exit(1);
        }
        std::string inputPath = pathIn;
        std::string dumpPath = pathOut;
        std::cout << "Dumping "+inputPath+" => "+dumpPath+"!\n";
        autoDump(inputPath,dumpPath,crc,true);
        exit(0);
    }
    if(strcmp(mode, "inspect") == 0){
        if (pathIn== nullptr){
            std::cerr <<"Specify an input\n";
            exit(1);
        }
        std::string inputPath = pathIn;
        std::cout << "Inspecting " << inputPath << "!\n";
        Parser *parser = getMatchingParser(inputPath,crc);
        if (parser == nullptr){
            std::cerr << "Invalid or unknown file!\n";
            exit(1);
        }
        ParserResult *result = parser->parseFile(inputPath,crc);
        if( result == nullptr){
            std::cerr << "Couldn't parse the given file :(\n";
            exit(1);
        }
        std::cout<<result->inspect();
        exit(0);
    }
    printHelp();
    return 1;
}
