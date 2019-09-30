//============================================================================
// Author: Octavian Marius Chincisan 2006
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef __SYSERRORS_H__
#define __SYSERRORS_H__

//-----------------------------------------------------------------------------
#define MK_ERR(err_)    (err_&0x800000000)
#define MK_WRN(err_)    (err_&0x40000000)

//-----------------------------------------------------------------------------
#define PROCESSED       1
#define OK              0
#define ERR_CONLOST     MK_WRN(1)


#define ERR_LOADREMLEVEL        MK_ERR(2)
#define INVLID_HTTPFILE         MK_ERR(3)




#endif //__SYSERRORS_H__