//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __UIMan_H__
#define __UIMan_H__

#include "basecont.h"
#include "_isystem.h"
#include "_irender.h"


//-----------------------------------------------------------------------------
extern SIZE FontsSzs[];

//-----------------------------------------------------------------------------
struct UiFont
{
    UV      _pelsz;
    SIZE    _chsz;
    Htex    _htex;
    HFONT   _hFont;
    HFONT	_hFontOld;
   	int		_base;
};

//-----------------------------------------------------------------------------
class UiControl //: public IGc
{
public:
    UiControl(){
        _pParent = _pSibling =_pChildren =0;
        _pInUse  = 0;
        _time    = -1;
        TRACEX("UiControl(%x)\r\n",this);
    }
    UiControl(int cmd, const RECT& pos, UiControl* parent=0);
    virtual ~UiControl();
    int         Paint(SystemData*  sysData);
    void        Clear();
    BOOL        Create(int cmd, const RECT& pos, UiControl* parent=0);
    BOOL        Createi(int cmd, int l, int t, int r, int b);
    void        SetColor(const CLR& txcolor, const CLR& bgcolor);
    void        SetColori(long txcolor, long bgcolor);
    void        SetText(const TCHAR* text, int align=-1);
    void        SetImage(const TCHAR* image);
    void        SetHImage(Htex& image);
    void        SetFont(int idx){_fidx = idx;}
    void        AutoHide(REAL timeout){_time = timeout;};
protected:
    void        _AddChildren(UiControl*  pUiControl);
    void        _AddSibling(UiControl*  pUiControl);
    void        _Paint(SystemData*  sysData);
    void        _Command(SystemData*  sysData);
public:
    Vtx         _defRect[4];
    V3          _t;
    V3          _s;
    V3          _r;
    int         _cmd;
    UiControl*  _pParent;
    UiControl*  _pSibling;
    UiControl*  _pChildren;
    DWORD       _drawFrame;
    BOOL        _mouseOver;
    BOOL        _mouseDown;
    Htex        _hTex;
    TCHAR       _text[128];
    CLR         _txtcolor;
    CLR         _bgcolor;
    int         _fidx;
    int         _align;
    UiControl*  _pInUse;
    REAL        _time;
};


//-----------------------------------------------------------------------------
// iin text coord of 140 x 64
class UiButton : public UiControl
{
public:
    UiButton(UINT cmd, const RECT& pos, UiControl* parent=0):UiControl(cmd, pos,parent){};
    virtual ~UiButton();
    int     Spin(SystemData*  sysData);
};

//-----------------------------------------------------------------------------
#define UI_CLOSE        1
#define UI_MAXIMIZE     2

//-----------------------------------------------------------------------------
struct SystemData;
class UIMan
{
public:

    friend class UiControl;
    UIMan(void);
    ~UIMan(void);
    void Clear();
    int  Create(const RndStruct* rs);
    int  Paint(SystemData*  sysData);
    void ResetCursor(SystemData*);
    SIZE GetScreenLinesCols(int font);
    void TextOutLC(int font, int xpos, int ypos, const TCHAR* txt, const CLR& tc, const CLR& bk=ZBLACK);
    void TextOut(int font, REAL xpos, REAL ypos, const TCHAR* txt, const CLR& tc, const CLR& bk=ZBLACK);
    UV&  GetVievPort(){return _viewport;}
    UV&  GetCharSize(int font){return _fonts[font%5]._pelsz;}
    int  GetScreenGridX(int font){
        return FontsSzs[font%4].cx;}
    int  GetScreenGridY(int font){
        return FontsSzs[font%4].cy;}
    UiControl* GetControl(int nCmd);

protected:
    void _ShowCursor(SystemData*  sysData);

private:    
    Vtx                 _defRect[5];
    UiFont              _fonts[5];
    Htex                _curArrow;
    V3                  _curPos;
    UV                  _viewport;
    vvector<UiControl*> _ctrls;
};



#endif // __UIMan_H__