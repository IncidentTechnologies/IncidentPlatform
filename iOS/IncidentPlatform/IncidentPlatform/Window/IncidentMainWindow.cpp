#include "IncidentMainWindow.h"
//#include "SenseDeviceKeyboard.h"    // for ASCII values

// Main Constructor
IncidentMainWindow::IncidentMainWindow() :
    m_dimension(NULL)
    //m_sense(NULL)
{ 
    
}

IncidentMainWindow::IncidentMainWindow(WindowFactory &wf, DimensionFactory &df/*, SenseFactory &sf*/) :
    m_dimension(NULL)
    //m_sense(NULL)
{
    RESULT r = R_SUCCESS;
    
    CRM(BuildWindow(wf), "IncidentMainWindow: Build Window Failed");
    CRM(BuildDimensionImplementation(df), "IncidentMainWindow: Build DimImp Failed");
    //CRM(BuildSense(sf), "IncidentMainWindow: Build Sense Failed");

    Validate();
    return;
Error:
    Invalidate();
    return;
}

// Main Destructor
IncidentMainWindow::~IncidentMainWindow() {
    // Kill The Input
    /*
    if(m_sense != NULL) {
        delete m_sense;
        m_sense = NULL;
    }
    */
    
    // Kill The Graphics
    if(m_dimension != NULL) {
        delete m_dimension;
        m_dimension = NULL;
    }
}

RESULT IncidentMainWindow::BuildWindow(WindowFactory& factory)
{        
    RESULT r = R_SUCCESS;
    
    m_pWindowImp = factory.MakeWindow();
    CNRM(m_pWindowImp, "IncidentMainWindow: MakeWindow returned a NULL object");

Error:
    return r;
}

RESULT IncidentMainWindow::BuildDimensionImplementation(DimensionFactory& factory)
{
    RESULT r = R_SUCCESS;
    
    DimensionImp *dimensionImp = factory.MakeDimension(m_pWindowImp);        
    CNRM(dimensionImp, "IncidentMainWindow: MakeDimension returned a NULL object");
    m_dimension = new Dimension(dimensionImp);

Error:
    return r;
}

/*
RESULT IncidentMainWindow::BuildSense(SenseFactory &factory) //, WindowImp *pWindowImp)
{
    RESULT r = R_SUCCESS;

    m_sense = new Sense(factory, m_pWindowImp);    
    CVRM(m_sense, "IncidentMainWindow: BuildSense failed due to invalid sense object");

Error:
        return r;
}
*/

RESULT IncidentMainWindow::StartMain(RESULT (*F[])(void*), int Functions_c)
{
    RESULT res = R_SUCCESS;
    bool done_f = false;
    
    // Call the window implementation's main function
    if(m_dimension != NULL && m_pWindowImp != NULL)
    {
        // initialize Dimension Imp            
        m_dimension->InitializeDimension(*m_pWindowImp);                                                         
    }
    else
    {
        return R_ERROR;
    }

    // Main Program Loop
    while(!done_f)
    {
        // Get Input First
        m_pWindowImp->HandleMessages();

        // Update Dimension
        // Test Code
        // m_dimension->RotateObjects();

        // Render
        m_dimension->RenderDimension();  

        /*
        // Sense Input State
        if(m_sense->UpdateSenseState() != 0) {
            done_f = true;
        }
        else {
            if(m_sense->CheckKey(ASCII_ESC) == 0) {
                done_f = true;   
            }
            else {
                // Here we allow for plugging in functions that depend on input                
                for(int i = 0; i < Functions_c; i++)
                {
                    // Pass whole sense object!
                    res = F[i](m_sense);
                }                
            }
        }
        */
    }

    m_pWindowImp->killWnd();
    return res;
}


