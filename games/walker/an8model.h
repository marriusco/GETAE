#ifndef __AN8MODEL_HH__
#define __AN8MODEL_HH__

#include "baseutils.h"
#include "an8filem.h"


class Ani8Model
{
public:
    Ani8Model(void){};
    ~Ani8Model(void){
    };
    
    int     Load(const TCHAR* fName);

private:
    AN8    _file;
    
};



#endif //