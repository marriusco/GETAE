#pragma once

#include "baselib.h"
#include <string>
using namespace std;

class Textbox
{
public:
    Textbox(int x, int y, const char* text, CLR& color, REAL live)
    {
        strcpy(_text, text);
        _live  = live;
        _color = color;
        _xy[0] = x;
        _xy[1] = y;
    }
    ~Textbox(void){};

    const char* Text(REAL frameTime){
        _live-=(frameTime*1000);
        if(_live < 500)
        {
            if(_color.r > 0)--_color.r;
            if(_color.g > 0)--_color.g;
            if(_color.b > 0)--_color.b;
        }
        if(_color.r==0 && _color.g==0 && _color.b==0)
            return 0;
        return _text;
    }
    int     _xy[2];
    char    _text[256];
    REAL    _live;
    CLR     _color;
};
