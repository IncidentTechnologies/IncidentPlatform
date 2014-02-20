#pragma once

// VSTPlugin is a plugin wrapper for VSTs in the iEngine Platform
// this way the user can create a VSTPlugin from any VST installed
// on the system or a plugin can be wrapped in this and shipped
// as is

#include "Plugin.h"
#include "SoundVST.h"

typedef struct VSTPluginUpdate 
    : public PluginUpdate
{    
    MSG msg;
} PLUGIN_VST_UPDATE;

class VSTPlugin :
    public plugin
{
public:

    // VSTPlugin Constructor may return invalid
    VSTPlugin(SoundImpFactory *pSIF, char *pszVSTPath) :
      plugin(pSIF),
      m_pSoundVST(NULL),
      m_pszVSTPath(pszVSTPath)
    {
        RESULT r = R_OK;

        // This is a plugin specific function implemented by each plugin
        CRM(PluginStart(), "plugin: PluginStart failed!");

        Validate();
        return;
Error:
        Invalidate();
        return;
    }

    VSTPlugin(SoundImp *pSI, char *pszVSTPath) :
      plugin(pSI),
      m_pSoundVST(NULL),
      m_pszVSTPath(pszVSTPath)
    {
        RESULT r = R_OK;
        
        // This is a plugin specific function implemented by each plugin
        CRM(PluginStart(), "plugin: PluginStart failed!");

        Validate();
        return;
Error:
        Invalidate();
        return;
    }

    ~VSTPlugin()
    {
      Invalidate();
    }

    RESULT PluginStart();
    RESULT Update(void *pUpdateContext);

    RESULT SaveToXMPFile(char *pszFilename);

    
private:
    char *m_pszVSTPath;
    SoundVST *m_pSoundVST;
};

RESULT TestVSTPlugin(Console *pConsole, char *pszVSTFilename);
RESULT TestVSTPluginXMPFile(Console *pConsole, char *pszVSTFilename, char *pszDestFile);