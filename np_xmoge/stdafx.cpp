// stdafx.cpp : source file that includes just the standard includes
// xmoge.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

 
#ifdef _DEBUG
    #pragma comment(lib, "..\\_lib\\extern\\jpg.lbb")    
    #pragma comment(lib, "..\\_lib\\baselibd.lib")
    #pragma comment(lib, "..\\_lib\\systemd.lib")
    #pragma comment(lib, "..\\_lib\\gebsplibd.lib")

    #pragma comment(lib, "..\\_lib\\sqstdlibd.lib")
    #pragma comment(lib, "..\\_lib\\squirreld.lib")
    #pragma comment(lib, "..\\_lib\\sqplusd.lib")
#else

    #pragma comment(lib, "..\\_lib\\extern\\jpg.lib")    
    #pragma comment(lib, "..\\_lib\\baselib.lib")
    #pragma comment(lib, "..\\_lib\\system.lib") 
    #pragma comment(lib, "..\\_lib\\gebsplib.lib") 

    #pragma comment(lib, "..\\_lib\\sqstdlib.lib")
    #pragma comment(lib, "..\\_lib\\squirrel.lib")
    #pragma comment(lib, "..\\_lib\\sqplus.lib")
#endif //

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")