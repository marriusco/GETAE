
// dllmain.h : Declaration of module class.

#include "mgplug_i.c"

class CmgplugModule : public CAtlDllModuleT< CmgplugModule >
{
public :
	DECLARE_LIBID(LIBID_mgplugLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MGPLUG, "{A696DA76-3E35-4704-9EB4-14B502823ACD}")
};

extern class CmgplugModule _AtlModule;
