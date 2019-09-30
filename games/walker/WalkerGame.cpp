//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#include "StdAfx.h"
#include "camera.h"
#include ".\walkergame.h"
#include "MovingLight.h"

System* Psys;

DPool<MovingLight>  DLights;

WalkerGame::WalkerGame(ISystem* psys, const TCHAR* pCmdLine):_scene(0)
{
    DLights.CreatePool(32);
    Psys = (System*)psys;
    if(pCmdLine)
        _tcscpy(_cmd_line, pCmdLine);
    else
        _cmd_line[0]=0;
}

WalkerGame::~WalkerGame(void)
{
    DLights.DestroyPool();
    Psys=0;
}


long WalkerGame::Proc(ISystem* ps, long m, long w, long l)
{
    switch(m)
    {
        case SYS_CONFIGURE:
            ((RndStruct*)l)->dmPelsWidth=640   ;
            ((RndStruct*)l)->dmPelsHeight=480;
        break;
        case SYS_START:
            do{
                int lerror = -1;
 
                _scene = new Scene();
                if(_cmd_line[0])
                {
                    TCHAR l[_MAX_PATH];
                    _tcscpy(l, _cmd_line);
                    if(_tcstok(l,_T(" -")))
                    {
                        TCHAR* plevelFile = _tcstok(0," -");
                        if(plevelFile)
                            lerror = _scene->LoadLevel(plevelFile);
                        else
                            lerror = _scene->LoadLevel("compiled/default.gbt");
                    }
                    else
                        lerror = _scene->LoadLevel("compiled/default.gbt");
                }
                    
                if(lerror == -1)
                {
                    delete _scene;
                    _scene = 0;
                    printf("\n cannot find file\n");
                    return -1; //shutdown
                }
                else{
                    Psys->SetMode(PLAY_MODE);
                    SystemData* psd = (SystemData*)l;
                    psd->_pRndStruct->bsetCapture=1;
                    psd->_pRndStruct->clipCursor=1;
                }
            }while(0);
            break;
        case SYS_CLOSEAPP:
            break;
        case SYS_EXIT:
            delete _scene;
            delete this;
            return 0; //stop
        case SYS_ANIMATE:
            if(_scene) _scene->Animate(((Camera*)w), (SystemData*)l);
            break;
        case SYS_RENDER_3D:
            if(_scene) _scene->Render((SystemData*)l, Z_POLYGON);
            break;
        case SYS_INPUT:
            if(_scene) _scene->OnKey(w, l);
            break;

    }
    return DEFAULT;
}
