// Generic Window Class
// Abstract class intended for general window purposes
// Abstracts implementation of the actual window or display area
// For all purposes window is used interchangably with display area.

#pragma once

#include "Window.h"
#include "Dimension.h"
#include "DimensionFactory.h"

class WindowMain :
    public Window
{
public:
    WindowMain() :
        m_dimension(NULL),
        m_windowImp(NULL)
    { 
    
    }

    ~WindowMain(){ };

    RESULT BuildWindow(WindowFactory& factory)
    {        
        m_windowImp = factory.MakeWindow();
        
        if(m_windowImp != NULL)
        {
            return SUCCEED;
        }
        else
        {
            return FAIL;
        }
    }

    RESULT BuildDimension(DimensionFactory& factory)
    {
        DimensionImp *dimensionImp = factory.MakeDimension();        

        if(dimensionImp != NULL)
        {
            // Give it to Dimension
            m_dimension = new Dimension(dimensionImp);
            return SUCCEED;
        }
        else
        {                        
            return FAIL;
        }
    }

    RESULT StartMain()
    {
        // Call the window implementation's main function
        if(m_dimension != NULL && m_windowImp != NULL)
        {
            // initialize Dimension Imp            
            m_dimension->InitializeDimension(*m_windowImp);

            // render one time to test
            m_dimension->RenderDimension();
            
            m_windowImp->main();
            return SUCCEED;
        }
        else
        {
            return FAIL;
        }
    }
    
private:
    // Window Implementation
    WindowImp *m_windowImp;

    // Dimension graphics engine instance
    Dimension *m_dimension;
};