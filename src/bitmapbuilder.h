#include "builder.h"

#include <png.h>
#include <stdio.h>

class BitmapBuilder : public Builder{
public:
    BitmapBuilder(ParserResult *reference);
    ~BitmapBuilder();
    virtual void build(std::string source, std::string pathOut) override;

private:

};
