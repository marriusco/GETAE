//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include <tchar.h>
#include "_irender.h"
#include "exports.h"

void S_Export2Script()
{

    
SQClassDef<StartStruct>("StartStruct").
    arr(&StartStruct::_renderSel,"renderSel",VAR_ACCESS_READ_WRITE).
    arr(&StartStruct::_inputSel,"inputSel",VAR_ACCESS_READ_WRITE).
    arr(&StartStruct::_audioSel,"audioSel",VAR_ACCESS_READ_WRITE).
    arr(&StartStruct::_fontFile,"fontFile",VAR_ACCESS_READ_WRITE).
    var(&StartStruct::_enableScript,"enableScript",VAR_ACCESS_READ_WRITE);


SQClassDef<RndStruct>("RndStruct").
    var(&RndStruct::retainedDraw,"retainedDraw",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::wndStyle,"wndStyle",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::fNear,"fNear",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::fFar,"fFar",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::hFov,"hFov",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::xPolyMode,"xPolyMode",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::wndStyle,"wndStyle",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::wndParent,"wndParent",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::pfDepthBits,"pfDepthBits",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::pfStencilBits,"pfStencilBits",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::bFullScreen,"bFullScreen",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::bgColor,"bgColor",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::clipCursor,"clipCursor",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::bsetCapture,"bKillFocus",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::dmPelsWidth,"dmPelsWidth",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::dmPelsHeight,"dmPelsHeight",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::dmBitsPerPel,"dmBitsPerPel",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::bIsActivated,"bActivated",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::fWidth,"fWidth",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::fHeight,"fHeight",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::nWidth,"nWidth",VAR_ACCESS_READ_WRITE).
    var(&RndStruct::nHeight,"nHeight",VAR_ACCESS_READ_WRITE);


SQClassDef<SystemData>("SystemData").
        var(&SystemData::_fps,"fps",VAR_ACCESS_READ_ONLY).
        var(&SystemData::_curtime,"curTime",VAR_ACCESS_READ_ONLY).
        var(&SystemData::_ticktime,"ticktime",VAR_ACCESS_READ_ONLY).
        var(&SystemData::_pauseTime,"pauseTime",VAR_ACCESS_READ_ONLY).
        var(&SystemData::_physFrame,"physframe",VAR_ACCESS_READ_ONLY).
        var(&SystemData::_drawFrame,"drawframe",VAR_ACCESS_READ_ONLY).
        var(&SystemData::_pauseTime,"pauseTime").
        func(&SystemData::Key,"Key").
        func(&SystemData::MouseBut,"MouseBut");


SQClassDef<V3>("V3").
                var(&V3::x,"x").
                var(&V3::y,"y").
                var(&V3::z,"z").
                staticFunc(V3Ctor,_T("constructor"));

SQClassDef<CLR>("CLR").var(&CLR::r,"r").var(&CLR::g,"g").var(&CLR::b,"b").var(&CLR::a,"a");

SQClassDef<UV>("UV").
            var(&UV::u,"u").
            var(&UV::v,"v").
            staticFunc(UVCtor,_T("constructor"));

SQClassDef<SIZE>("SIZE").var(&SIZE::cx,"cx").var(&SIZE::cy,"cy");

SQClassDef<Pos>("Pos").var(&Pos::_right,"right")
                      .var(&Pos::_up,"up")
                      .var(&Pos::_fwd,"fwd")
                      .var(&Pos::_pos,"pos")
                      .var(&Pos::_euler,"euler")
                      .var(&Pos::_rot,"rot")
                      .var(&Pos::_scale,"zoom");


 
    SQClassDef<Vtx>("Vtx").var(&Vtx::_xyz,"xyz").
        var(&Vtx::_nrm,"nrm").
        varAsUserPointer(&Vtx::_uv,"uv").
        var(&Vtx::_rgb,"rgb").
        var(&Vtx::_user,"user");


     SQClassDef<RenderVx>("RenderVx").
                    func(&RenderVx::getv,"getv").
                    func(&RenderVx::getu,"getu").
                    func(&RenderVx::getn,"getn").
                    func(&RenderVx::getb,"getb").
                    func(&RenderVx::getc,"getc").
                    func(&RenderVx::setv,"setv").
                    func(&RenderVx::setu,"setu").
                    func(&RenderVx::setn,"setn").
                    func(&RenderVx::setb,"setb").
                    func(&RenderVx::setc,"setc").
                    staticFunc(VxCtor,_T("constructor"));
                    


SQClassDef<M4>("M4");

SQClassDef<Box>("Box").
            var(&Box::_min,_T("min")).
            var(&Box::_max,_T("max"));//HTEX



SQClassDef<Itape>(true,"Itape").func(&Itape::SetVolume,"SetVolume").
                                func(&Itape::D3Pos,"SetPos");


SQClassDef<Htex>("Htex").var(&Htex::hTex,"hTex").
                         var(&Htex::glTarget,"glTarget").
                         var(&Htex::envMode,"envMode").
                         var(&Htex::genST,"genST");
                           

SQClassDef<Irender>(true,"Irender").
                        func(&Irender::Push,"Push").
                        func(&Irender::DbgLine,"DbgLine").
                        func(&Irender::Pop,"Pop").
                        func(&Irender::Rotate,"Rotate").
                        func(&Irender::RotateTo,"RotateTo").
                        func(&Irender::Scale,"Scale").
                    	func(&Irender::SetTransform,"SetTransform").
	                    func(&Irender::ResetTransform,"ResetTransform").
                        func(&Irender::Translate,"Translate").
                        func(&Irender::Begin,"Begin").
                        func(&Irender::BeginPrim,"BeginPrim").
                        func(&Irender::End,"End").
                        func(&Irender::Color4,"Color4").
                        func(&Irender::RenderVertex,"RenderVertex").
                        func(&Irender::Vertex2,"Vertex2").
	                    func(&Irender::Vertex3,"Vertex3").
	                    func(&Irender::Texture2,"Texture2").
                        func(&Irender::BindTex1,"BindTex1").
                        func(&Irender::BindTex2,"BindTex2").
                        func(&Irender::BindTex3,"BindTex3").
                        func(&Irender::SetArrayPtrs,"SetArrayPtrs").
                        func(&Irender::ResetArrayPtrs,"ResetArrayPtrs").
                        func(&Irender::SetArray,"SetArray").
                        func(&Irender::ResetArray,"ResetArray").
                        func(&Irender::DrawArrays,"DrawArrays").
                        func(&Irender::DisableTextures,"DisableTextures").
                        func(&Irender::Blend, "Blend").
                        func(&Irender::UnBlend, "UnBlend").
                        func(&Irender::EnableRenderMaterialLighting,"EnableRenderMaterialLighting");



SQClassDef<Camera>("Camera").
                func(&Camera::ViewMatrix,"ViewMatrix").
                func(&Camera::SetPos,"SetPos").
                func(&Camera::GetFinderHeight,"GetFinderHeight").
                func(&Camera::GetFinderWidth,"GetFinderWidth").
                func(&Camera::GetFinderDepth,"GetFinderDepth").
                func(&Camera::SetFarAndFov,"SetFarAndFov");


    SQClassDef<SIZE>("SIZE").var(&SIZE::cx,"cx").
                             var(&SIZE::cy,"cy");


    SQClassDef<POINT>("Point").var(&POINT::x,"x").
                               var(&POINT::y,"y");

    SQClassDef<RECT>("Rect").var(&RECT::left,"l").
                             var(&RECT::right,"r").
                             var(&RECT::top,"t").
                             var(&RECT::bottom,"b");

    SQClassDef<Plane>("Plane").var(&Plane::_n,"n").
                                   var(&Plane::_c,"c").
                                   var(&Plane::_t,"t").
                                   var(&Plane::_u,"u");


    SQClassDef<RenderLight>("RenderLight").var(&RenderLight::_flags,"flags").
                                            var(&RenderLight::_pos,"pos").
                                            var(&RenderLight::_direction,"dir").
                                            var(&RenderLight::_radius,"").
                                            var(&RenderLight::_colorAmbient,"amb").                 
                                            var(&RenderLight::_colorDiffuse,"diff").
                                            var(&RenderLight::_colorSpecular,"spec").
                                            var(&RenderLight::_intens,"int").
                                            var(&RenderLight::_cutOff,"cutoff").
                                            var(&RenderLight::_fallOff,"falloff").
                                            var(&RenderLight::_constAttenuation,"cattn").
                                            var(&RenderLight::_linearAttenuation,"lattn").
                                            var(&RenderLight::_quadratAttenuation,"qattn").
                                            var(&RenderLight::_userdata,"ud");

    SQClassDef<RenderMaterial>("RenderMaterial").var(&RenderMaterial::_flags,"flags").
                                                var(&RenderMaterial::_colorAmbient,"amb").
                                                var(&RenderMaterial::_colorDiffuse,"diff").
                                                var(&RenderMaterial::_colorEmmisive,"emis").
                                                var(&RenderMaterial::_colorSpecular,"spec").
                                                var(&RenderMaterial::_shiness,"shine");
   
    SQClassDef<RenderFog>("RenderFog").var(&RenderFog::_gnear,"ner").
                                    var(&RenderFog::_gfar,"far").
                                    var(&RenderFog::_dens,"dens").
                                    var(&RenderFog::_clr,"clr").
                                    var(&RenderFog::_flag,"flag").
                                    var(&RenderFog::_clip,"clip");


    SQClassDef<UIMan>("UIMan").
                func(&UIMan::Clear,"Clear").
                func(&UIMan::Create,"Create").
                func(&UIMan::Paint,"Paint").
                func(&UIMan::ResetCursor,"ResetCursor").
                func(&UIMan::TextOut,"TextOut").
                func(&UIMan::GetVievPort,"GetVievPort").
                func(&UIMan::GetCharSize,"GetCharSize").
                func(&UIMan::GetScreenGridY,"GetScreenGridY").
                func(&UIMan::GetScreenGridX,"GetScreenGridX").
                func(&UIMan::GetControl,"GetControl");


    SQClassDef<UiControl>("UiControl").
                func(&UiControl::Createi,"Create").
                func(&UiControl::SetColori,"SetColor").
                func(&UiControl::SetText,"SetText").
                func(&UiControl::SetImage,"SetImage").
                func(&UiControl::SetHImage,"SetHImage").
                func(&UiControl::SetFont,"SetFont");

    SQClassDef<GServer>("GServer").
                func(&GServer::GetIp,"GetIp").
                func(&GServer::GetPort,"GetPort");



    SQClassDef<LevelMan>("LevelMan").
                func(&LevelMan::SetupAddrs,"SetupAddrs").
                func(&LevelMan::CheckLocally,"CheckLocally").
                func(&LevelMan::Download,"Download").
                func(&LevelMan::GetResourceCount,"GetResourceCount").
                func(&LevelMan::GetResourceAt,"GetResourceAt").
                func(&LevelMan::QueryMaster,"QueryMaster").
                func(&LevelMan::GetServerAt,"GetServerAt");


    SQClassDef<System>("System").
        func(&System::Stop,"Stop").
        func(&System::GenTex,"GenTex").
        func(&System::GenTexFile,"GenTexFile").
        func(&System::GenTexFont,"GenTexFont").
        func(&System::DeleteTexFont,"DeleteTexFont").
        func(&System::DeleteTex,"DeleteTex"). 
        func(&System::AddSoundFile,"AddSoundFile").
        func(&System::RemoveSoundFile,"RemoveSoundFile").
        func(&System::RemoveSound,"RemoveSound").
        func(&System::GetTape,"GetTape").
        func(&System::PlayPrimary,"PlayPrimary").
        func(&System::StopPlay,"StopPlay").
        func(&System::PlayPrimaryFile,"PlayPrimaryFile").
        func(&System::StopPlayFile,"StopPlayFile").
        func(&System::SetSpeeds,"SetSpeeds").
        func(&System::IsKeyDown,"IsKeyDown"). 
        func(&System::Moves,"Moves").
        func(&System::Keys,"Keys").
        func(&System::GetPressedKey,"GetPressedKey").
        func(&System::SetMode,"SetMode").
        func(&System::GetMode,"GetMode").
        func(&System::GetCamera,"GetCamera").
        func(&System::SetHudMode,"SetHudMode").
        func(&System::GetHudMode,"SetHudMode").
        func(&System::IgnoreInput,"IgnoreInput").
        func(&System::GetUIMan,"GetUIMan").
        func(&System::GetSysData,"GetSysData").
        func(&System::ToggleFullScreen,"ToggleFullScreen").
        func(&System::Print,"Print").
        func(&System::New,"New").
        func(&System::Delete,"Delete").
        func(&System::Render,"GetRenderSys");


    SQClassDef<FileBrowser>("FileBrowser").
            func(&FileBrowser::BroseDir,"BroseDir").
            func(&FileBrowser::GetAt,"GetAt");

    SQClassDef<UIInputMap>("InputMap").
            func(&UIInputMap::GetAKM, "GetAKM").
            func(&UIInputMap::SetAKM, "SetAKM").
            func(&UIInputMap::GetName, "GetName").
            func(&UIInputMap::SetName, "SetName");

    SQClassDef<InputSys>("InputSys").
        func(&InputSys::IsKeyDown,"IsKeyDown").
        func(&InputSys::GetPressedKey,"GetPressedKey").
        func(&InputSys::IgnoreInput,"IgnoreInput").
        func(&InputSys::SetMappedKey,"SetMappedKey").
        func(&InputSys::GetMappedKey,"GetMappedKey");

}
/*         

exporting base class
SQClassDef<Base>("Base")
    .func(&Base::BaseFunction, "BaseFunction");
SQClassDef<D1>("D1", "Base") //here we tell that it's derived
    .func(&D1::DFunction, "DFunction");


#endif //            

}



/*
Fixed a bug in the code... it should be as follows:

// forums do not allow correct syntax
#include vector
#include sqplus.h

using namespace SqPlus;

class ByteArray
{
public:
    vector(unsigned char) m_data; // forums do not allow correct syntax

    ByteArray() {}

    ByteArray(int size)
    {
       m_data.resize(size);
    }

    static int construct(HSQUIRRELVM v)
    {
       StackHander sa(v);
       int paramCount = sa.GetParamCount();
       if (paramCount == 2)
       {
          return PostConstruct(v, new ByteArray(sa.GetInt(2)), ByteArray::release);
       }
       else
       {
          return sq_throwerror(v, _T("Invalid Constructor arguments"));
       }
    }

    static int release(SQUserPointer up, SQInteger size)
    {
       SQ_DELETE_CLASS(ByteArray);
    }

    int _set(HSQUIRRELVM v)
    {
       StackHandler sa(v);
       int index = sa.GetInt(2);
       m_data[index] = sa.GetInt(3);
       return m_data[index];
    }

    int _get(SQUIRRELVM v)
    {
       StackHandler sa(v);
       int index = sa.GetInt(2);
       return m_data[index];
    }
};

DECLARE_INSTANCE_TYPE(ByteArray)


void initScript(void)
{
    SquirrelVM::Init();

}


Script:

local data = ByteArray(6);
data[0] = 0x01

//export base class
SQClassDef<Base>("Base")
    .func(&Base::BaseFunction, "BaseFunction");
SQClassDef<D1>("D1", "Base") //here we tell that it's derived
    .func(&D1::DFunction, "DFunction");


*/
