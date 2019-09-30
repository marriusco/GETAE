// Player.cpp: implementation of the CPlayer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "baselib.h"
#include "Player.h"


CPlayer::CPlayer()
{
    _leaf     = -1;
    _vis      = 0xFFFFFFFF;
    _flags    = 0;
    _ping     = 0;
    _cliState = CLI_OFFLINE;
}

CPlayer::~CPlayer()
{

}


void    CPlayer::OnMessage(counted_ptr<Buffer>& rB)
{
}


void    CPlayer::SetPos(const Pos& o)
{
    ((Pos&)*this) = o;
    Dirty(1);
    
    History h;
    h.pos._pos   = this->_pos;
    h.pos._euler = this->_euler;
    h.time       = ge_gettick();
    _history.Put(h);
}

void    CPlayer::VisFlag(int index, int set)
{
    if(set)
        _vis |= (1<<index);
    else
        _vis &= ~(1<<index);
}


void    CPlayer::Reinitialize()
{
    _leaf     = -1;
    _vis      = 0xFFFFFFFF;
    _flags    = 0;
    _history.Reset();
}

void    CPlayer::SetPosPoint(REAL x, REAL y, REAL z)
{
    this->_pos.x=x;   
    this->_pos.y=y;   
    this->_pos.z=z;

    History h;
    h.pos._pos   = this->_pos;
    h.pos._euler = this->_euler;
    h.time       = ge_gettick();
    _history.Put(h);
}

