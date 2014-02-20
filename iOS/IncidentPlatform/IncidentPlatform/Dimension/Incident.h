#pragma once

// Incident is the main instantiation of the iEngine
// this object will contain:
// - Window - Platform specific code for OS details
// - Dimension - Graphics and object manipulations
// - Sound - Sound engine and manipulations
// - Sense - Input manipulations etc

class Incident
{
public:
    Incident() :
      m_pWindow(NULL),
      m_pDimension(NULL),
      m_pSense(NULL)
    {
        
    }

    ~Incident()
    {
        if(m_pWindow != NULL)
        {
            delete m_pWindow;
            m_pWindow = NULL;
        }
        if(m_pDimension != NULL)
        {
            delete m_pDimension;
            m_pDimension = NULL;
        }
        if(m_pSense != NULL)
        {
            delete m_pSense;
            m_pSense = NULL;
        }
    }

private:
    Window *m_pWindow;
    Dimension *m_pDimension;
    
    Sense *m_pSense;          // Moving to multiple sense devices
};