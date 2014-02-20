#ifdef __WIN32__
#include "WindowNPAPIWin32.h"

// NPAPI Includes  (need to also add the SDK path to the includes)
#include "npapi.h"
#include "npupp.h"

// NPAPI Interface Calls
NPNetscapeFuncs NPNFuncs;

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* pFuncs)
{
    NPError npe = NPERR_NO_ERROR;

    if(pFuncs == NULL || pFuncs->size < sizeof(NPPluginFuncs))
    {
        return NPERR_INVALID_FUNCTABLE_ERROR;
    }

    // Assign our functions here
    pFuncs->version = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;
    pFuncs->newp          = NPP_New;
    pFuncs->destroy       = NPP_Destroy;
    pFuncs->setwindow     = NPP_SetWindow;
    pFuncs->newstream     = NPP_NewStream;
    pFuncs->destroystream = NPP_DestroyStream;
    pFuncs->asfile        = NPP_StreamAsFile;
    pFuncs->writeready    = NPP_WriteReady;
    pFuncs->write         = NPP_Write;
    pFuncs->print         = NPP_Print;
    pFuncs->event         = NPP_HandleEvent;
    pFuncs->urlnotify     = NPP_URLNotify;
    pFuncs->getvalue      = NPP_GetValue;
    pFuncs->setvalue      = NPP_SetValue;
    pFuncs->javaClass     = NULL;

Error:
    return npe;
}

NPError WINAPI NP_Initialize(NPNetscapeFuncs *pFuncs)
{
    NPError npe = NPERR_NO_ERROR;

    int SizeOfNPNetscapeFuncs = sizeof(NPNetscapeFuncs);

    if(pFuncs == NULL /*|| pFuncs->size > sizeof(NPNetscapeFuncs)*/)
    {
        return NPERR_INVALID_FUNCTABLE_ERROR;
    }

    if(HIBYTE(pFuncs->version) > NP_VERSION_MAJOR)
    {
        return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }

    // Copy over the NPNetscapeFuncs structure (must be a better way to do this)
    NPNFuncs.size                    = pFuncs->size;
    NPNFuncs.version                 = pFuncs->version;
    NPNFuncs.geturlnotify            = pFuncs->geturlnotify;
    NPNFuncs.geturl                  = pFuncs->geturl;
    NPNFuncs.posturlnotify           = pFuncs->posturlnotify;
    NPNFuncs.posturl                 = pFuncs->posturl;
    NPNFuncs.requestread             = pFuncs->requestread;
    NPNFuncs.newstream               = pFuncs->newstream;
    NPNFuncs.write                   = pFuncs->write;
    NPNFuncs.destroystream           = pFuncs->destroystream;
    NPNFuncs.status                  = pFuncs->status;
    NPNFuncs.uagent                  = pFuncs->uagent;
    NPNFuncs.memalloc                = pFuncs->memalloc;
    NPNFuncs.memfree                 = pFuncs->memfree;
    NPNFuncs.memflush                = pFuncs->memflush;
    NPNFuncs.reloadplugins           = pFuncs->reloadplugins;
    NPNFuncs.getJavaEnv              = NULL;
    NPNFuncs.getJavaPeer             = NULL;
    NPNFuncs.getvalue                = pFuncs->getvalue;
    NPNFuncs.setvalue                = pFuncs->setvalue;
    NPNFuncs.invalidaterect          = pFuncs->invalidaterect;
    NPNFuncs.invalidateregion        = pFuncs->invalidateregion;
    NPNFuncs.forceredraw             = pFuncs->forceredraw;
    NPNFuncs.getstringidentifier     = pFuncs->getstringidentifier;
    NPNFuncs.getstringidentifiers    = pFuncs->getstringidentifiers;
    NPNFuncs.getintidentifier        = pFuncs->getintidentifier;
    NPNFuncs.identifierisstring      = pFuncs->identifierisstring;
    NPNFuncs.utf8fromidentifier      = pFuncs->utf8fromidentifier;
    NPNFuncs.intfromidentifier       = pFuncs->intfromidentifier;
    NPNFuncs.createobject            = pFuncs->createobject;
    NPNFuncs.retainobject            = pFuncs->retainobject;
    NPNFuncs.releaseobject           = pFuncs->releaseobject;
    NPNFuncs.invoke                  = pFuncs->invoke;
    NPNFuncs.invokeDefault           = pFuncs->invokeDefault;
    NPNFuncs.evaluate                = pFuncs->evaluate;
    NPNFuncs.getproperty             = pFuncs->getproperty;
    NPNFuncs.setproperty             = pFuncs->setproperty;
    NPNFuncs.removeproperty          = pFuncs->removeproperty;
    NPNFuncs.hasproperty             = pFuncs->hasproperty;
    NPNFuncs.hasmethod               = pFuncs->hasmethod;
    NPNFuncs.releasevariantvalue     = pFuncs->releasevariantvalue;
    NPNFuncs.setexception            = pFuncs->setexception;

Error:
    return npe;
}

NPError OSCALL NP_Shutdown()
{
    return NPERR_NO_ERROR;
}

// ***************************************************
// Implement the NPP functions called by the navigator
// ***************************************************

NPError NPP_Initialize(void)
{
    return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
    /*empty stub*/
}

NPError NPP_New(NPMIMEType pluginType, 
                NPP instance, 
                uint16 mode, 
                int16 argc, 
                char* argn[], 
                char* argv[], 
                NPSavedData* saved)
{
    NPError npe = NPERR_NO_ERROR;

    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    WindowNPAPIWin32 *pPlugin = new WindowNPAPIWin32(instance, &NPNFuncs);
    if(pPlugin == NULL)
    {
        return NPERR_OUT_OF_MEMORY_ERROR;
    }

    instance->pdata = (void*)pPlugin;

Error:
    return npe;
}

NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
    NPError npe = NPERR_NO_ERROR;

    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    WindowNPAPIWin32 *pPlug = (WindowNPAPIWin32 *)instance->pdata;

    if(pPlug != NULL)
    {
        pPlug->Shut();
        delete pPlug;
    }

Error:
    return npe;
}

NPError NPP_SetWindow(NPP instance, NPWindow* window)
{
    NPError npe = NPERR_NO_ERROR;

    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

    if(window == NULL)
    {
        return NPERR_GENERIC_ERROR;
    }

    WindowNPAPIWin32 *pPlug = (WindowNPAPIWin32 *)instance->pdata;

    if(pPlug == NULL)
    {
        return NPERR_GENERIC_ERROR;
    }

    if(!pPlug->IsInitialized() && window->window != NULL)
    {
        // The window has just been created
        if(!pPlug->Initialize(window))
        {
            delete pPlug;
            pPlug = NULL;
            return NPERR_MODULE_LOAD_FAILED_ERROR;
        }
    }
    else if(pPlug->IsInitialized() && window->window == NULL)
    {
        // Window destroyed
        return NPERR_NO_ERROR;
    }
    else if(pPlug->IsInitialized() && window->window != NULL)
    {
        // Window resized
        return NPERR_NO_ERROR;
    }
    else if(!pPlug->IsInitialized() && window->window == NULL)
    {
        // Should never happen do nothing
        return NPERR_NO_ERROR;
    }

Error:
    return npe;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16* stype)
{
    NPError npe = NPERR_NO_ERROR;

    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

Error:
    return npe;
}

NPError NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    NPError npe = NPERR_NO_ERROR;

    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

Error:
    return npe;
}

int32 NPP_WriteReady(NPP instance, NPStream* stream)
{
    int32_t rv = 0xffffffff;

    if(instance == NULL)
    {
        return NPERR_INVALID_INSTANCE_ERROR;
    }

Error:
    return rv;
}

int32 NPP_Write(NPP instance, NPStream* stream, int32 offset, int32 len, void* buffer)
{
    int32_t rv = len;

    if(instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

Error:
    return rv;
}

void NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    if(instance == NULL)
        return;

Error:
    return;
}

void NPP_Print(NPP instance, NPPrint* platformPrint)
{
    if(instance == NULL)
        return;

Error:
    return;
}

int16 NPP_HandleEvent(NPP instance, void* Event)
{
    int16 rv = 0;

    if(instance == NULL)
        return 0;

    WindowNPAPIWin32 *pPlug = (WindowNPAPIWin32 *)instance->pdata;

    if(pPlug)
        pPlug->HandleEvent(Event);

Error:
    return rv;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData) {
    if(instance == NULL)
        return;

Error:
    return;
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    NPError npe = NPERR_NO_ERROR;

    if(instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

    WindowNPAPIWin32 *pPlug = (WindowNPAPIWin32 *)instance->pdata;

    if(pPlug == NULL)
        return NPERR_GENERIC_ERROR;

    switch(variable) {
        case NPPVpluginNameString: {
            *((char **)value) = "iEngineNPAPIWin32";
        } break;

        case NPPVpluginDescriptionString: {
            *((char **)value) = "iEngine NPAPI Win32 Implementation";
        } break;

        case NPPVpluginScriptableNPObject:{
            // *(NPObject **)value = pPlug->GetScriptableObject();
        } //break;

        default: {
            return NPERR_GENERIC_ERROR;
        } break;
    }

Error:
    return npe;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value) {
    NPError npe = NPERR_NO_DATA;

    if(instance == NULL)
        return NPERR_INVALID_INSTANCE_ERROR;

Error:
    return npe;
}

#endif // #ifdef __WIN32__