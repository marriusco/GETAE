//============================================================================
// Author: Octavian Marius Chincisan Sept 2006
// Zalsoft Inc 1999-2006
//============================================================================
#pragma once

#include "system.h"
#include "Scene.h"



class WalkerGame
{
public:
    WalkerGame(ISystem* psys, const TCHAR* pCmdLine);
    ~WalkerGame(void);

    long Proc(ISystem* ps, long m, long w, long l);

private:
    Scene*      _scene;
    TCHAR       _cmd_line[_MAX_PATH];//const TCHAR* pCmdLine
};

extern System* Psys;