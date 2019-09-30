#include ".\messages.h"
#include "../system/system.h"



void Msg::Render(System* psystem, SystemData* psd)
{
    FOREACH(vector<Textbox*>, _tboxes, pptb)
    {
        const char* ptxt = (*pptb)->Text(psd->_ticktime);
        if(ptxt)
            psystem->GetUIMan()->TextOutLC(2, (*pptb)->_xy[0], (*pptb)->_xy[1], ptxt, (*pptb)->_color, ZBLACK);
        else 
        {
            delete ptxt;
            pptb =  _tboxes.erase(pptb);
            break;
        }
    }
}