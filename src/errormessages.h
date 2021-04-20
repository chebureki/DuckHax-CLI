#pragma once

#include <stdexcept>

#include "parsinghelper.h"

#define CANT_READ(path) std::runtime_error(formatString("Couldn't open %s for reading!\n",path))
#define CANT_WRITE(path) std::runtime_error(formatString("Couldn't open %s for writing!\n",path))

#define CANT_MKDIR(path) std::runtime_error(formatString("Couldn't create %s!\n",path))
#define NOT_A_DIR(path) std::runtime_error(formatString("%s is not a directory!\n",path))

#define CANT_PARSE(path) std::runtime_error(formatString("Couldn't parse %s!\n",path))
