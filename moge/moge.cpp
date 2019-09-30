//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================
#include "utils.h"
#include "exports.h"

System* PGs;
BOOL    Exception;
//---------------------------------------------------------------------------------------
// main function
#ifdef _WINDOWS
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#else
    int main(int argc, char* argv[])
#endif //
{
    BOOL        halt = 1;
    LPSTR       pCmd = 0;

#ifndef _WINDOWS
    HINSTANCE hInstance = ::GetModuleHandle(argv[0]);
    
    HWND hCon = ::FindWindow(0, argv[0]); // micro-soft way of getting console handler
    RECT rt; ::GetWindowRect(hCon, &rt);
    MoveWindow(hCon,GetSystemMetrics(SM_CXSCREEN)-(rt.right-rt.left),
                    GetSystemMetrics(SM_CYSCREEN)-(rt.bottom-rt.top),
                    (rt.right-rt.left),
                    (rt.bottom-rt.top),1);

    if(argc)   
        pCmd = argv[1];
#else
    pCmd  = lpCmdLine;
#endif
    Initializer init;
    BOOL        helpshown = 0;
    if(init.Rebase() )
    {
        char         cwd[260];
        char         loco[256]; 
        char         filename[256]; 
        static  char cmdEntered[128];

        //DebugBreak();

        getcwd(cwd,260); 
        printf("cwd %s\r\n\r\n",cwd);
        while(1)
        {
            
            while(0==pCmd)
            {
                if(!helpshown)
                {
                    printf("Run Moge with command line parameter or type in at moge prompt:\n"
                            "-A 'Getic Dynamic Level' '*.dll' located in curent directory\n"
                            "-A 'Getic Script' '*.gs' located in scripts/ directory (do not type the path)\n"
                            "-'q' to quit\n"
                            "type either: 'walker.dll' or 'bspwalk.gs'<cr>\n"
                            
                            "moge>");
                    helpshown=1;
                }
                else
                    printf("\nmoge>");
                
                pCmd = gets(cmdEntered);
            }
            if(!_tcscmp(pCmd,"Q") || !_tcscmp(pCmd,"q"))
                break;
            printf("\r\n%s\r\n",pCmd);            
            strcpy(loco, pCmd);

            char uloco[256];
            strcpy(uloco, pCmd);
            for(int i=0;i < strlen(uloco);i++)
            {
                if(uloco[i]>='a' && uloco[i]<='z')
                    uloco[i]-=0x20;
            }
            printf("%s\n",uloco);
            char* pgsScr=0, *pdll=0;

            if((pgsScr=strstr(uloco,".GS")) || (pdll=strstr(uloco,".GLL")) || (pdll=strstr(uloco,".DLL")))
            {
                char* ptok = strtok(loco," -"); // ddd and -level.gbt
                if(ptok)
                {
                    strcpy(filename, ptok);
                }
                else
                {
                    strcpy(filename, loco);
                }

                if(!FindFile(filename))
                {
                    printf("\ncannot find %s\r\n", loco);
                    pCmd = 0;
                    continue;
                }


                //
                // create system and loading a script
                //
                
                {

                    //ShowWindow(hCon,SW_HIDE);

                    if(pgsScr)
                    {
                        try{
                            S_Export2Script();
                            string& procName =  SquirrelExecuteMain(loco);
                            if(!procName.empty())
                            {
                                PGs = System::LoadScript(hInstance, pCmd, procName.c_str());
                                if(PGs)
                                {
                                    PGs->Run(); 
                                }
                            }
                            halt = 0;
                        }
                        catch(SquirrelError &e)
                        {
                            Exception = TRUE;
                            printfunc(0, e.desc);
                            _LOG(LOG_ERROR,"Script: %s  \r\n", e.desc);
                        }
#ifndef _DEBUG
                        catch(...)
                        {
                            Exception = TRUE;
                            printfunc(0, "Exception (...) \r\n");
                            _LOG(LOG_ERROR,"Exception (...) \r\n");
                        }
#endif //
                    }
                    //
                    // create system and load a dll game
                    //
                    else if(pdll)
                    {
                        PGs = System::LoadDLL(hInstance, pCmd, pCmd);
                        if(PGs)
                        {
                            PGs->Run();
                        }
                    }
#ifndef _WINDOWS
                    ShowWindow(hCon,SW_SHOW);
                    SetFocus(hCon);
#endif //
                }
            }
            else 
            {
                printf("%s is not a script or dll file!\r\n", pCmd);
            }
            System::Destroy(PGs);
            
            //SquirrelVM::Cleanup();
            SquirrelVM::Shutdown();
            SquirrelVM::Init();

            pCmd=0;
        }
    }

    if(halt || System::_lastError[0])
    {
        if(System::_lastError[0])
            printfunc(0, System::_lastError);
        if(!pCmd)
            printfunc(0, "moge <filename.dll> | <filename.gs>.\r\n");
        printfunc(0, "\n\n\n\n\n\n Press any key to exit");
        getch();        
    }
	return 0;
}

