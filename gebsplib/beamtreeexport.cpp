//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================
#include <tchar.h>
#include "system.h"
#include "basecont.h"
#include "beamtree.h"
#include "beamtreesi.h" //script interfaces
#include "inireader.h"
#include "beamtreeexport.h"
#include "bspfilestr.h"

BOOL	BeamTree::Export2Script(System* ps, HINSTANCE hi)
{
    PS = ps;
    _LOG(LOG_INFO,"BeamTree::Configure()\r\n");
    TCHAR appName[260];
    GetModuleFileName(  hi, appName, 260);
    char* pl = &appName[strlen(appName)-1];
    while(pl!=appName && *pl!='.')--pl;
    if(*pl=='.')*pl=0;
    _tcscat(appName,".ini");    
    CIniReader ir(appName, 0);
    int exportFoos = ir.Readi(_T("geticbsp"),_T("export"), 1);   


    SQClassDef<BeamTree>("BeamTree").
                        func(&BeamTree::LoadLevel,_T("LoadLevel")).
                        func(&BeamTree::Render,_T("Render")).
                        func(&BeamTree::Animate,_T("Animate")).
                        func(&BeamTree::Clear,_T("Clear")).
                        func(&BeamTree::IsLoaded,_T("IsLoaded")).
                        func(&BeamTree::WantMessages,_T("WantMessages")).
                        staticFunc(BspCtor,_T("constructor"));

    //
    // temporary usage until I figure out how to pass array members
    //
    
    SQClassDef<TexPack>("TexPack"); 
    
    SQClassDef<BTF_Item>(_T("BTF_Item")).
                        var(&BTF_Item::_type,_T("type")).                      
                        var(&BTF_Item::_pos,_T("pos")).                
                        var(&BTF_Item::_dir,_T("dir")).
                        var(&BTF_Item::_box,_T("box")).
                        var(&BTF_Item::_flags,_T("flags")).
                        arr(&BTF_Item::_name,_T("name")).
                        arr(&BTF_Item::_catname,_T("catname")).
                        var(&BTF_Item::_areavis,_T("areavis")).
                        var(&BTF_Item::_texp,_T("texp"));
                        /*

    SQClassDef<_BTF_LightBulb>("_BTF_LightBulb").
                                var(&_BTF_LightBulb::_specAngle, _T("specAngle")).     
                                var(&_BTF_LightBulb::_lmIntensity, _T("lmIntensity")).     
                                var(&_BTF_LightBulb::_radius, _T("radius")).     
                                var(&_BTF_LightBulb::_halloRadMin, _T("halloRadMin")).     
                                var(&_BTF_LightBulb::_halloRadMax, _T("halloRadMax")).     
                                var(&_BTF_LightBulb::_mimDist, _T("mimDist")).     
                                var(&_BTF_LightBulb::_maxDist, _T("maxDist")).     
                                arr(&_BTF_LightBulb::__unused, _T("unused")).     
                                arr(&_BTF_LightBulb::_colorD, _T("colorD")).     
                                arr(&_BTF_LightBulb::_colorE, _T("colorE")).     
                                arr(&_BTF_LightBulb::_colorS, _T("colorS")).     
                                var(&_BTF_LightBulb::_fallOff, _T("fallOff")).     
                                arr(&_BTF_LightBulb::_attenuation, _T("attenuation"));     
*/

    /*
    SQClassDef<BTF_Trigger>("BTF_Trigger");
    SQClassDef<BTF_StartItem>("BTF_StartItem");
    SQClassDef<BTF_GameItem>("BTF_GameItem");
    SQClassDef<BTF_Void>("BTF_Void");
    SQClassDef<BST_SceneInfo>("BST_SceneInfo");
    
    SQClassDef<BTF_Details>("BTF_Details");
    SQClassDef<BTF_ItemCat>("BTF_ItemCat");
    */



    BeamTree::_configured=1;

    return 0;
}
