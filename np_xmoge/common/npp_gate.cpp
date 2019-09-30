#include "pluginbase.h"
#include "baseutils.h"
#include "../nsxmogePeer.h"
extern NPNetscapeFuncs NPNFuncs;

static bool existence = false;
static mutex       __mutex;
//-------------------------------------------------------------------------------------------------
// here the plugin creates a plugin instance object which 
// will be associated with this newly created NPP instance and 
// will do all the neccessary job
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, 
                char* argn[], char* argv[], NPSavedData* saved)
{   
    TRACEX("----------->NPP_New \n");
    AutoLock    al(&__mutex);
    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    

    CNS_Moge * plugin = new CNS_Moge(instance);
    if(plugin)
    {
        plugin->AddRef();
        instance->pdata = (void *)((nsPluginInstanceBase*)plugin);
        existence = true;
        return 0;
    }
    return NPERR_INVALID_INSTANCE_ERROR;
}

//-------------------------------------------------------------------------------------------------
// here is the place to clean up and destroy the nsPluginInstance object
NPError NPP_Destroy (NPP instance, NPSavedData** save)
{
    AutoLock    al(&__mutex);
    TRACEX("----------->NPP_Destroy \n");
    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    NPError rv = NPERR_NO_ERROR;
    nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
    if(plugin != NULL) 
    {
        plugin->shut();
    }
    existence = false;
    return rv;
}

//-------------------------------------------------------------------------------------------------
// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow (NPP instance, NPWindow* pNPWindow)
{    
    TRACEX("----------->NPP_SetWindow %x\n", pNPWindow);
    AutoLock    al(&__mutex);

    if(instance == 0)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }
    if(pNPWindow == 0)
    {
        return NPERR_GENERIC_ERROR;
    }

    nsPluginInstanceBase* plugin = (nsPluginInstanceBase*)instance->pdata;
    if(plugin == NULL) 
    {
        return NPERR_GENERIC_ERROR;
    }

    plugin->SetWindow(pNPWindow);
    return 0;//
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
{
    TRACEX("----------->NPP_NewStream \n");
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return NPERR_GENERIC_ERROR;

  NPError rv = plugin->NewStream(type, stream, seekable, stype);
  return rv;
}

int32 NPP_WriteReady (NPP instance, NPStream *stream)
{
    TRACEX("----------->NPP_WriteReady \n");
  if(instance == NULL)
    return 0x0fffffff;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return 0x0fffffff;

  int32 rv = plugin->WriteReady(stream);
  return rv;
}

int32 NPP_Write (NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{   
    TRACEX("----------->NPP_Write \n");
  if(instance == NULL)
    return len;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return len;

  int32 rv = plugin->Write(stream, offset, len, buffer);
  return rv;
}

NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason)
{
    TRACEX("----------->NPP_DestroyStream\n");
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return NPERR_GENERIC_ERROR;

  NPError rv = plugin->DestroyStream(stream, reason);
  return rv;
}

void NPP_StreamAsFile (NPP instance, NPStream* stream, const char* fname)
{
    TRACEX("----------->NPP_StreamAsFile \n");
  if(instance == NULL)
    return;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return;

  plugin->StreamAsFile(stream, fname);
}

void NPP_Print (NPP instance, NPPrint* printInfo)
{
    TRACEX("----------->NPP_Print \n");
  if(instance == NULL)
    return;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return;

  plugin->Print(printInfo);
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{

    TRACEX("----------->NPP_URLNotify\n");

  if(instance == NULL)
    return;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return;

  plugin->URLNotify(url, reason, notifyData);
}

NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    TRACEX("----------->NPP_GetValue %d \n", variable);

  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return NPERR_GENERIC_ERROR;

  NPError rv = plugin->GetValue(variable, value);
  return rv;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    TRACEX("----------->NPP_SetValue %d \n", variable);

  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return NPERR_GENERIC_ERROR;

  NPError rv = plugin->SetValue(variable, value);
  return rv;
}

int16	NPP_HandleEvent(NPP instance, void* event)
{
    TRACEX("----------->NPP_HandleEvent\n");

  if(instance == NULL)
    return 0;

  nsPluginInstanceBase * plugin = (nsPluginInstanceBase *)instance->pdata;
  if(plugin == NULL) 
    return 0;

  uint16 rv = plugin->HandleEvent(event);
  return rv;
}

#ifdef OJI
jref NPP_GetJavaClass (void)
{
  return NULL;
}
#endif

/**************************************************/
/*                                                */
/*                     Mac                        */
/*                                                */
/**************************************************/

// Mac needs these wrappers, see npplat.h for more info

#ifdef XP_MAC

NPError	Private_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char* argn[], char* argv[], NPSavedData* saved)
{
  NPError rv = NPP_New(pluginType, instance, mode, argc, argn, argv, saved);
  return rv;	
}

NPError Private_Destroy(NPP instance, NPSavedData** save)
{
  NPError rv = NPP_Destroy(instance, save);
  return rv;
}

NPError Private_SetWindow(NPP instance, NPWindow* window)
{
  NPError rv = NPP_SetWindow(instance, window);
  return rv;
}

NPError Private_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
{
  NPError rv = NPP_NewStream(instance, type, stream, seekable, stype);
  return rv;
}

int32 Private_WriteReady(NPP instance, NPStream* stream)
{
  int32 rv = NPP_WriteReady(instance, stream);
  return rv;
}

int32 Private_Write(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer)
{
  int32 rv = NPP_Write(instance, stream, offset, len, buffer);
  return rv;
}

void Private_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
  NPP_StreamAsFile(instance, stream, fname);
}


NPError Private_DestroyStream(NPP instance, NPStream* stream, NPError reason)
{
  NPError rv = NPP_DestroyStream(instance, stream, reason);
  return rv;
}

int16 Private_HandleEvent(NPP instance, void* event)
{
  int16 rv = NPP_HandleEvent(instance, event);
  return rv;
}

void Private_Print(NPP instance, NPPrint* platformPrint)
{
  NPP_Print(instance, platformPrint);
}
 
void Private_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
  NPP_URLNotify(instance, url, reason, notifyData);
}

jref Private_GetJavaClass(void)
{
  return NULL;
}

NPError Private_GetValue(NPP instance, NPPVariable variable, void *result)
{
  NPError rv = NPP_GetValue(instance, variable, result);
  return rv;
}

NPError Private_SetValue(NPP instance, NPNVariable variable, void *value)
{
  NPError rv = NPP_SetValue(instance, variable, value);
  return rv;
}

#endif //XP_MAC
