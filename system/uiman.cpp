//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================

#include "_iinput.h"
#include "uiman.h"
#include "system.h"

//-------------------------------------------------------------------------------------------------
static UIMan* PUiManager;

//-------------------------------------------------------------------------------------------------
SIZE FontsSzs[] = {
        {140,64},
        {120,40},
        {100,32},
        {60,20},
        {40,15}
    };

//-------------------------------------------------------------------------------------------------
static unsigned char DefGry[]={
255,128,128,128,128,128,128,255,
128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,
255,128,128,128,128,128,128,255
};
//-------------------------------------------------------------------------------------------------
static unsigned char DefCursor[]={
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 128, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 128, 255, 128, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 255, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
128, 255, 255, 0, 255, 255, 255, 0  , 255, 255, 255, 0, 255, 255, 255, 128,  
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 255, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 128, 255, 128, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 255, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 128, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};

//-------------------------------------------------------------------------------------------------
static unsigned char CloseButt[]={
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 , 
255,  96, 255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 0,   128, 
255, 192,  96, 255, 192, 192, 192, 192, 192, 192, 192, 192, 255,  96, 192, 128, 
255, 192, 192,  96, 255, 192, 192, 192, 192, 192, 192, 255,  96, 192, 192, 128, 
255, 192, 192, 192,  96, 255, 192, 192, 192, 192, 255,  96, 192, 192, 192, 128, 
255, 192, 192, 192, 192,  96, 255, 192, 192, 255,  96, 192, 192, 192, 192, 128, 
255, 192, 192, 192, 192, 192,  96, 255, 255,  96, 192, 192, 192, 192, 192, 128, 
255, 192, 192, 192, 192, 192, 192,  96,  96, 192, 192, 192, 192, 192, 192, 128, 
255, 192, 192, 192, 192, 192, 255,  96,  0,  255, 192, 192, 192, 192, 192, 128, 
255, 192, 192, 192, 192, 255,  96, 192, 192,  96, 255, 192, 192, 192, 192, 128, 
255, 192, 192, 192, 255,  96, 192, 192, 192, 192,  96, 255, 192, 192, 192, 128, 
255, 192, 192, 255,  96, 192, 192, 192, 192, 192, 192,  96, 255, 192, 192, 128, 
255, 192, 255,  96, 192, 192, 192, 192, 192, 192, 192, 192,  96, 255, 192, 128, 
255, 255,  96, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,  96, 255, 128, 
255,  96, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192,  96, 128, 
255, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
};

//-------------------------------------------------------------------------------------------------
static unsigned char FullScrBut[]={
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 128, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 192, 192, 128, 
255, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 192, 255, 128, 192, 128, 
255, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 
};

//-------------------------------------------------------------------------------------------------
UIMan::UIMan(void)
{
    assert(0==PUiManager);
    PUiManager = this;
}

//-------------------------------------------------------------------------------------------------
UIMan::~UIMan(void)
{
    PUiManager = 0;
}

//-------------------------------------------------------------------------------------------------
void UIMan::Clear()
{
    Irender* pR = PSystem->Render();
    if(pR)
    {
        pR->SetCurrent();
        for(int j=0; j<5; j++)
        {
	        pR->UseFontBitmaps(0, 0, 0, _fonts[j]._base);
	        DeleteObject(_fonts[j]._hFont);
        }
        PSystem->DeleteTex(&_curArrow,1);
        while(_ctrls.size())
        {
            _ctrls[0]->Clear();
            delete (_ctrls[0]);
        }
    }
}

//-------------------------------------------------------------------------------------------------
int  UIMan::Create(const RndStruct* rs)
{
    Irender* pR = PSystem->Render();
    
    pR->SetCurrent();
    for(int j=0; j<4; j++)
    {
        _fonts[j]._chsz.cy = rs->nHeight/(REAL)FontsSzs[j].cy; 
        _fonts[j]._chsz.cx = rs->nWidth/(REAL)FontsSzs[j].cx;  

        _fonts[j]._pelsz.v = rs->fHeight/(REAL)FontsSzs[j].cy;
        _fonts[j]._pelsz.u = rs->fWidth/(REAL)FontsSzs[j].cx;

        _fonts[j]._hFont = CreateFont( -_fonts[j]._chsz.cy,
		    				            _fonts[j]._chsz.cx,        
			    			            0,    
				    		            0,     
					    	            0, 
						                FALSE,   
						                FALSE,   
						                FALSE,   
						                ANSI_CHARSET,    
						                OUT_TT_PRECIS,   
						                CLIP_DEFAULT_PRECIS,  
						                ANTIALIASED_QUALITY,  
						                FF_DONTCARE,
						                "Arial");
        _fonts[j]._base = pR->UseFontBitmaps(_fonts[j]._hFont, 0, 255, 0);
    }

    REAL dy = rs->fHeight/64.0;
    REAL dx = rs->fWidth/80.0;

    _defRect[0]._xyz    = V3(-dx,-dy,0);
    _defRect[0]._uv[0]  = UV(0,1);
    
    _defRect[1]._xyz    = V3(dx,-dy,0);
    _defRect[1]._uv[0]  = UV(1,1);
    
    _defRect[2]._xyz    = V3(dx,dy,0);
    _defRect[2]._uv[0]  = UV(1,0);

    _defRect[3]._xyz    = V3(-dx,dy,0);
    _defRect[3]._uv[0]  = UV(0,0);

    _curArrow = *PSystem->GenTex("$c", 16, 16, 1, (const char*)DefCursor, TEX_NORMAL);

    if(rs->dwVpStyle & 1)
    {
        RECT rt = {134, 60, 140, 64};
        UiControl* pC =  new UiControl(-1, rt, 0);
        pC->SetColor(ZRED, CLR(255,255,255,1));
        pC->SetHImage(*PSystem->GenTex("$x", 16, 16, 1, (const char*)CloseButt, TEX_NORMAL));
    }


    if(rs->dwVpStyle & 2)
    {
        RECT rtc = {128, 60, 134, 64};
        UiControl* pC =  new UiControl(-2, rtc, 0);
        pC->SetColor(ZRED, CLR(255,255,255,1));
        pC->SetHImage(*PSystem->GenTex("$o", 16, 16, 1, (const char*)FullScrBut, TEX_NORMAL));
    }


    return 0;
}

//-------------------------------------------------------------------------------------------------
int  UIMan::Paint(SystemData*  sysData)
{
    Irender* pR = PSystem->Render();    
    _viewport.v = sysData->_pRndStruct->fHeight;
    _viewport.u = sysData->_pRndStruct->fWidth;
    
    if(PSystem->GetMode() == SET_MODE && _ctrls.size())
    {
        FOREACH(vvector<UiControl*>, _ctrls, pctrl)
        {
            (*pctrl)->Paint(sysData);
        }
    }
    pR->DisableTextures(1);
    _ShowCursor(sysData);
    return 1;
}

//-------------------------------------------------------------------------------------------------
void  UIMan::_ShowCursor(SystemData*  sysData)
{
    Irender* pR = PSystem->Render();
    _curPos.z = -(sysData->_pRndStruct->fNear+.02);
    REAL xc = _viewport.u/2;
    REAL yc = _viewport.v/2;

    if(PSystem->GetMode() == SET_MODE)
    { 
        _curPos.x += sysData->_pMouse[3] * .01;
        _curPos.y -= sysData->_pMouse[4] * .01;
        CLAMPVAL(_curPos.x, -xc, xc);
        CLAMPVAL(_curPos.y, -yc, yc);
    }
    else
    {
        _curPos.y=_curPos.x=0;
    }
    pR->Push();
    pR->Translate(_curPos);
    pR->Color4((char)255,(char)255,(char)255,0);
    pR->Render(DS_BLEND_HALLO|DS_DEEPTEST_OFF, &_curArrow, 1, _defRect, 4, Z_QUADS|VX_TX1);
    pR->Pop();
}

//-------------------------------------------------------------------------------------------------
void UIMan::ResetCursor(SystemData*  sysData)
{
    _curPos.x = 0;
    _curPos.y = 0;
}

//-------------------------------------------------------------------------------------------------
void UIMan::TextOut(int font, REAL xpos, REAL ypos, const TCHAR* txt, 
                    const CLR& tc, const CLR& bk)
{
    if(txt && *txt)
    {
        Irender* pR = PSystem->Render();
        pR->Color(ZWHITE);
        pR->DisableTextures();
        pR->TextOut(_fonts[font]._base, 
                    xpos-_viewport.u/2.0, ypos-_viewport.v/2.0, 
                    txt, tc);    
    }
}

SIZE UIMan::GetScreenLinesCols(int font)
{
    if(font < sizeof(FontsSzs)/sizeof(FontsSzs[0]))
        return FontsSzs[font];
    return FontsSzs[0];
}

//-------------------------------------------------------------------------------------------------
void UIMan::TextOutLC(int font, int xpos, int ypos, const TCHAR* txt, 
                        const CLR& tc, const CLR& bk)
{
    if(txt && *txt)
    {
        Irender* pR = PSystem->Render();
        SIZE fsz   = FontsSzs[font];
        REAL fxpos = _viewport.u / (REAL)fsz.cx; // one line
        REAL fypos = _viewport.v / (REAL)fsz.cy; // one char
        fxpos *= xpos;
        fypos *= ypos;
        pR->TextOut(_fonts[font]._base, 
                    fxpos-_viewport.u/2.0, fypos-_viewport.v/2.0, 
                    txt, tc);    
    }
}


//-------------------------------------------------------------------------------------------------
UiControl* UIMan::GetControl(int nCmd)
{
    FOREACH(vvector<UiControl*>, _ctrls, pctrl)
    {
        if((*pctrl)->_cmd == nCmd)
            return (*pctrl);
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------
//pos comes in ints from x(0-140) y=)(0-64) (smallest font size)
UiControl::UiControl(int cmd, const RECT& pos, UiControl* parent)
{
    TRACEX("UiControl(%x)\r\n",this);
    _pInUse = 0;
    Create(cmd, pos, parent);
}

//-------------------------------------------------------------------------------------------------
// until we learn how to pass from script a rect
BOOL   UiControl::Createi(int cmd, int l, int t, int r, int b)
{
    _pInUse = 0;
    RECT rt = {l,t,r,b};
    return Create(cmd, rt, 0);
}

//-------------------------------------------------------------------------------------------------
BOOL   UiControl::Create(int cmd, const RECT& pos, UiControl* parent)
{
    _pInUse = 0;
    RndStruct* rs = PSystem->DrawProps();

    REAL xunit = PUiManager->_fonts[0]._pelsz.u;
    REAL yunit = PUiManager->_fonts[0]._pelsz.v;

    _t.x = (-rs->fWidth/2)  + (pos.left * xunit);
    _t.y = (-rs->fHeight/2) + (pos.top  * yunit);
    _t.z = -(rs->fNear + .11);

    REAL dx = (pos.right-pos.left)*xunit;
    REAL dy = (pos.bottom-pos.top)*yunit;

    _defRect[0]._xyz    = V3(0,0,0);
    _defRect[0]._uv[0]  = UV(0,1);
    _defRect[1]._xyz    = V3(dx,0,0);
    _defRect[1]._uv[0]  = UV(1,1);
    _defRect[2]._xyz    = V3(dx,dy,0);
    _defRect[2]._uv[0]  = UV(1,0);
    _defRect[3]._xyz    = V3(0,dy,0);
    _defRect[3]._uv[0]  = UV(0,0);
    _s = V1;
    _drawFrame = -1;
    _pChildren  = 0;
    _pSibling   = 0;
    _cmd        = cmd;
    _mouseOver  = 0;
    _mouseDown  = 0;
    _text[0]    = 0;
    _bgcolor    = ZLIGHTGRAY;
    _txtcolor   = ZWHITE;
    _hTex       = *PSystem->GenTex("$d", 8, 8, 1, (const char*)DefGry, TEX_NORMAL);
    _fidx       = 2;
    if(_pParent = parent)
        parent->_AddChildren(this);
    PUiManager->_ctrls << this;
    return 1;
}

//-------------------------------------------------------------------------------------------------
void  UiControl::_AddChildren(UiControl*  pUiControl)
{
    if(_pChildren)
        _pChildren->_AddSibling(pUiControl);
    else
    {
        _pChildren = pUiControl;
    }
}

//-------------------------------------------------------------------------------------------------
void UiControl::_AddSibling(UiControl*  pUiControl)
{
    if(_pSibling)
        _pSibling->_AddSibling(pUiControl);
    else
    {
        _pSibling = pUiControl;
    }
}

//-------------------------------------------------------------------------------------------------
int UiControl::Paint(SystemData*  sysData)
{
    if(_drawFrame == sysData->_drawFrame) 
        return 1;
    _drawFrame = sysData->_drawFrame;

    UiControl*  pWalk = _pChildren;
    while(pWalk)
    {
        pWalk->Paint(sysData);
        pWalk = pWalk->_pChildren;
    }
    pWalk = _pSibling;
    while(pWalk)
    {
        pWalk->Paint(sysData);
        pWalk = pWalk->_pSibling;
    }
    this->_Paint(sysData);
    this->_Command(sysData);
    return 1;
}

//-------------------------------------------------------------------------------------------------
void UiControl::_Paint(SystemData*  sysData)
{
    Irender* pR = PSystem->Render();
    pR->Push();
    pR->Translate(_t);
    pR->Scale(_s);
    pR->Rotate(_r);
    CLR tc    = _txtcolor;
    CLR color = _bgcolor;
    if(_mouseOver)
    {
        tc.interpolate(tc, ZWHITE, .7);
        color.interpolate(color, ZWHITE, .7);
    }
    pR->Color(color);
    pR->DisableTextures(1);
    DWORD bl = 0;

    if(_bgcolor.a != 255) bl = DS_BLEND_HALLO|DS_DEEPTEST_OFF;

    pR->Render(bl, &_hTex, 1, _defRect, 4, Z_QUADS|VX_TX1);
    pR->Pop();

    REAL rw   = _defRect[1]._xyz.x-_defRect[0]._xyz.x;
    REAL rh   = _defRect[2]._xyz.y-_defRect[0]._xyz.y;
    REAL xpos;
    REAL yaBit = PUiManager->_fonts[_fidx]._pelsz.v/8;
    if(_align==-1)
        xpos = _t.x+PUiManager->_fonts[_fidx]._pelsz.u;
    if(_align==0)
        xpos = _t.x + (rw - (_tcslen(_text)*PUiManager->_fonts[_fidx]._pelsz.u))/2;
    else
        xpos = _t.x + (rw - (_tcslen(_text)*PUiManager->_fonts[_fidx]._pelsz.u));
    REAL ypos = _t.y + (  rh - (PUiManager->_fonts[_fidx]._pelsz.v) )/2 + yaBit;
    pR->DisableTextures(1);
    pR->TextOut(PUiManager->_fonts[_fidx]._base, xpos, ypos, _text, tc);

    if(_time>0)
    {
        REAL time = _time;
        _time -= sysData->_ticktime;
        if(_time < 4.0000)
        {
            --_bgcolor.a;
        }
        if(_bgcolor.a==0)
        {
            delete this;
            return;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void UiControl::_Command(SystemData*  sysData)
{
    V3& cp = PUiManager->_curPos;

    _pInUse = this;

    if(cp.x > _t.x && cp.x < (_defRect[1]._xyz.x-_defRect[0]._xyz.x)+_t.x &&
       cp.y > _t.y && cp.y < (_defRect[2]._xyz.y-_defRect[1]._xyz.y)+_t.y )
    {
        if(sysData->_pMouse[B_1])
        {
            _mouseDown = 1;
        }
        else if(_mouseDown)
        {
            PSystem->GetInputSys()->Unaquire();
            PSystem->PostProc(SYS_UICOMMAND, _cmd, 0, _mouseDown);
            _mouseDown = 0;
        }
        if(_mouseOver==0)
        {
            _mouseOver=1;
            PSystem->SendMessage(SYS_UIMOUSEENTER, _cmd, _mouseOver);
        }
    }
    else if(_mouseOver)
    {
        _mouseOver=0;
        PSystem->SendMessage(SYS_UIMOUSEENTER, _cmd, _mouseOver);
    }
    _pInUse = 0;
}

//-------------------------------------------------------------------------------------------------
void   UiControl::SetText(const TCHAR* text, int align)
{
    _tcscpy(_text, text);
    _align = align;
}

//-------------------------------------------------------------------------------------------------
void   UiControl::SetImage(const TCHAR* image)
{
    if(_hTex.hTex)
        PSystem->DeleteTex(&_hTex, 0);
    _hTex  = *PSystem->GenTexFile(image, TEX_NORMAL);
}

//-------------------------------------------------------------------------------------------------
void   UiControl::SetHImage(Htex& image)
{
    _hTex = image;
}

void   UiControl::SetColori(long txcolor, long bgcolor)
{
    CLR t((DWORD)txcolor);
    CLR b((DWORD)bgcolor);
    SetColor(t, b);
}

//-------------------------------------------------------------------------------------------------
void   UiControl::SetColor(const CLR& t, const CLR& b)
{
    _txtcolor = t;
    _bgcolor  = b;
}

//-------------------------------------------------------------------------------------------------
UiControl::~UiControl()
{
    TRACEX("~UiControl(%x)\r\n",this);
    assert(_pInUse==0); // control is in use from command
    Clear();
    if(PUiManager)
    {
        FOREACH(vvector<UiControl*>, PUiManager->_ctrls, pctrl)
        if(*pctrl == this){
            PUiManager->_ctrls.erase(pctrl);
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------
void  UiControl::Clear()
{
    PSystem ?  PSystem->DeleteTex(&_hTex, 0) : 0;    
}
