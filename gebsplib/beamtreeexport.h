//============================================================================
// Author: Octavian Marius Chincisan 2006 March - 2006 Sept
// Zalsoft Inc 1999-2006
//============================================================================
#ifndef _BTREE_EXPORT_H__
#define _BTREE_EXPORT_H__

#include "bspfilestr.h"
#include "../system/exports.h"


// until I figure out how to pass in out arrays we use this getter
// wrapper  that acceeses the class layout by offsets inside it

DECLARE_ARRAY_TYPE(REAL,3,VAR_TYPE_USER_POINTER);
DECLARE_ARRAY_TYPE(REAL,4,VAR_TYPE_USER_POINTER);
DECLARE_ARRAY_TYPE(BYTE,4,VAR_TYPE_USER_POINTER);
DECLARE_ARRAY_TYPE(char,4,VAR_TYPE_USER_POINTER);
DECLARE_ARRAY_TYPE(char,32,VAR_TYPE_STRING);
DECLARE_ARRAY_TYPE(BYTE,32,VAR_TYPE_STRING); 


DECLARE_INSTANCE_TYPE(TexPack);
DECLARE_INSTANCE_TYPE(BTF_Item);
/*
DECLARE_INSTANCE_TYPE(BTF_LightBulb);
DECLARE_INSTANCE_TYPE(BTF_Trigger);
DECLARE_INSTANCE_TYPE(BTF_StartItem);
DECLARE_INSTANCE_TYPE(BTF_GameItem);
DECLARE_INSTANCE_TYPE(BST_SceneInfo);
DECLARE_INSTANCE_TYPE(BTF_Details);
DECLARE_INSTANCE_TYPE(BTF_ItemCat);
DECLARE_INSTANCE_TYPE(BTF_Void);
*/







#endif //