#pragma once

#include "baselib.h"
#include "textbox.h"
#include <vector>
using namespace std;

class System;
struct SystemData; 
class Msg
{
public:
    Msg(void)
    {
    }
    ~Msg(void)
    {
	    FOREACH(vector<Textbox*>, _tboxes, pptb)
            delete (*pptb);
        _tboxes.clear();
    }

    void Clear()
    {
	    FOREACH(vector<Textbox*>, _tboxes, pptb)
            delete (*pptb);
        _tboxes.clear();
        _tboxes.push_back(new Textbox(3,1,"Escape to leave, A,S,D,W to fly, Click to Move Around!" , ZBLUELIGHT, 4000));

    }

    void Tmsg(REAL x, REAL y, const char* text, CLR& color, int time)
    {
        _tboxes.push_back(new Textbox(x,y,text, color, time));
    }

    void Render(System* psystem, SystemData* psd);

private:
    vector<Textbox*> _tboxes;
};
