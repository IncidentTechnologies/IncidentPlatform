#pragma once

// The plugin interface is one meant to allow people to implement 
// plugins that will be immediately compatible with any application
// developed on the iEngine platform.

#include "SoundImpFactory.h"
#include "SmartBuffer.h"

typedef enum PluginTypes
{
    PLUGIN_TYPE_DEFAULT,
    PLUGIN_TYPE_VST,
    PLUGIN_TYPE_INVALID
} PLUGIN_TYPES;

typedef enum PluginSubTypes
{
    PLUGIN_SUBTYPE_INSTRUMENT,
    PLUGIN_SUBTYPE_EFFECT,
    PLUGIN_SUBTYPE_INVALID
} PLUGIN_SUBTYPE;

typedef struct PluginUpdate
{
    int id;
} PLUGIN_UPDATE;

class plugin :
    public valid
{
public:
    plugin(SoundImpFactory *pSIF, PLUGIN_SUBTYPE pst = PLUGIN_SUBTYPE_INVALID, char *pszDescription = NULL) :
      m_psbDescription(NULL),
      m_PluginSubType(pst),
      m_pSoundImpFactory(pSIF),
      m_pSoundImp(NULL)
    {
        RESULT r = R_OK;

        m_psbDescription = new SmartBuffer(pszDescription);
        CNRM(m_psbDescription, "plugin: Failed to allocate smartbuffer");
        
        CNRM(m_pSoundImpFactory, "Plugin requires a valid SoundImpFactory");
        m_pSoundImp = m_pSoundImpFactory->MakeSoundImp();

        Validate();
        return;
Error:
        Invalidate();
        return;
    }

    plugin(SoundImp *pSI, PLUGIN_SUBTYPE pst = PLUGIN_SUBTYPE_INVALID, char *pszDescription = NULL) :
      m_psbDescription(NULL),
      m_PluginSubType(pst),
      m_pSoundImpFactory(NULL),
      m_pSoundImp(pSI)
    {
        RESULT r = R_OK;

        m_psbDescription = new SmartBuffer(pszDescription);
        CNRM(m_psbDescription, "plugin: Failed to allocate smartbuffer");
        
        CNRM(m_pSoundImp, "Plugin requires a valid SoundImp Object");   

        Validate();
        return;
Error:
        Invalidate();
        return;
    }

    ~plugin()
    {
        if(m_pSoundImpFactory != NULL)
        {
            delete m_pSoundImpFactory;
            m_pSoundImpFactory = NULL;
        }

        if(m_pSoundImp != NULL)
        {
            m_pSoundImp->Release();
            delete m_pSoundImp;
            m_pSoundImp = NULL;
        }
    }

    virtual RESULT PluginStart() = 0;
    virtual RESULT Update(void *pUpdateContext) = 0;


protected:
    SoundImp *m_pSoundImp;
    SoundImpFactory *m_pSoundImpFactory;

    PLUGIN_SUBTYPE m_PluginSubType;
    SmartBuffer *m_psbDescription;

};