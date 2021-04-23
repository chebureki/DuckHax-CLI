#include "autodumper.h"
#include "parsinghelper.h"
#include "errormessages.h"

#include <iostream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <string.h>


void printHelp(){
    std::cout<<"USAGE: DuckHax \n"<<
                "\tdump -i FILE/DIR -o PATHOUT/OUTPUT \n"<<
                    //"\t\t -c automatically convert files\n"<<
                    "\n"<<
                "\tinspect -i FILE \n"<<
                    "\n"<<
                "\tbuild -r REFERENCE -i FILE -o OUTPUT \n"
                    << "\n";
    ;
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
    char *pathRef = nullptr;
    while((c = getopt(argc-1 , argv+1,"i:o:r:")) != -1){
        switch(c){
        case 'i':
            pathIn = optarg;
            break;
        case 'o':
            pathOut = optarg;
            break;
        case 'r':
                pathRef = optarg;
                break;
        case '?':
            printHelp();
            exit(1);
            break;
        }
    }
    char *mode = argv[1];
    std::string inputPath = pathIn;
    std::string dumpPath = pathOut;
    CRC32Lookup crc{};
    if (strcmp(mode, "dump")==0){

        if (pathOut == nullptr || pathIn == nullptr){
            std::cerr <<"Specify input and output!\n";
            exit(1);
        }
        std::cout<< formatString("Dumping %s -> %s!\n",inputPath,dumpPath);
        autoDump(inputPath,dumpPath,crc,true);
        exit(0);
    }
    if(strcmp(mode, "inspect") == 0){
        if (pathIn== nullptr){
            std::cerr <<"Specify an input\n";
            exit(1);
        }
        std::cout << "Inspecting " << inputPath << "!\n";
        ZounaClasses type = autoDetectFile(inputPath,crc);
        Parser *parser = getMatchingParser(type,crc);
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

    //TODO:
    //this code is duplicated multiple times, what if we did the same stuff
    //for all operations and only do the mode specific stuff?
    if(strcmp(mode,"build") == 0){
        if (pathIn== nullptr){
            std::cerr <<"Specify an input\n";
            exit(1);
        }
        if (pathOut== nullptr){
            std::cerr <<"Specify an input\n";
            exit(1);
        }
        if (pathRef == nullptr){
            std::cerr <<"Specify a reference\n";
            exit(1);
        }
        std::string refPath = pathRef;
        ZounaClasses type = autoDetectFile(refPath,crc);
        Parser *parser = getMatchingParser(type,crc);
        if (parser == nullptr){
            std::cerr << "Invalid or unknown file!\n";
            exit(1);
        }
        ParserResult *result = parser->parseFile(refPath,crc);
        if( result == nullptr){
            std::cerr << "Couldn't parse the given file :(\n";
            exit(1);
        }
        result->build(std::string(pathIn),std::string(pathOut));
        exit(0);
    }
    printHelp();
    return 1;
}
