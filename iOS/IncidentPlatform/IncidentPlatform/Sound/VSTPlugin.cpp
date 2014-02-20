#include "VSTPlugin.h"

RESULT VSTPlugin::PluginStart()
{
    RESULT r = R_OK;

    // First get the VST up and running
    m_pSoundVST = new SoundVST(m_pSoundImp, m_pszVSTPath);


Error:
    return r;
}

RESULT VSTPlugin::Update( void *pUpdateContext )
{
    RESULT r = R_OK;

    VSTPluginUpdate *pVPU = static_cast<VSTPluginUpdate*>(pUpdateContext);
    CBRM((pVPU->id == PLUGIN_TYPE_VST), "VSTPlugin: Update failed due to invalid update structure");
    CRM(m_pSoundVST->Update(), "VSTPlugin: Update failed due to SoundVST::Update failure");
    CRM(m_pSoundVST->HandleEditorMessage(pVPU->msg), "VSTPlugin: Update failed due to HandleEditorMessage failure");

Error:
    return r;
}

// This will create an XMP tree and then save it to a file later
RESULT VSTPlugin::SaveToXMPFile(char *pszFilename)
{
    RESULT r = R_OK;

    //TODO: DO THIS
    //XMPTree *pVSTXMPTree = new XMPTree();



Error:
    return r;
}

RESULT TestVSTPlugin(Console *pConsole, char *pszVSTFilename)
{
    RESULT r = R_OK;

    SoundImpFactory sif(SOUND_IMP_DIRECTSOUND);
    VSTPlugin *pVSTPlugin = new VSTPlugin(&sif, pszVSTFilename); 

    // Need to tighten this pattern up
    VSTPluginUpdate VPU;
    VPU.id = PLUGIN_TYPE_VST;

    // Create a wanna be message loop for the test function
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {                       
        VPU.msg = msg;
        pVSTPlugin->Update((void*)(&VPU));   
    }

Error:
    return r;
}

RESULT TestVSTPluginXMPFile(Console *pConsole, char *pszVSTFilename, char *pszDestFile)
{
    RESULT r = R_OK;

    SoundImpFactory sif(SOUND_IMP_DIRECTSOUND);
    VSTPlugin *pVSTPlugin = new VSTPlugin(&sif, pszVSTFilename); 
    CNRM(pVSTPlugin, "Failed to allocate VSTPlugin object");
    CVRM(pVSTPlugin, "VST plug in was invalid");    
    CRM(pVSTPlugin->SaveToXMPFile(pszDestFile), "VST Plugin failed to save to XMP file");


Error:
    return r;
}