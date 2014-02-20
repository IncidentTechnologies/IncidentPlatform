// Incident Main Window
// This is the main display area and container object for the
// Incident Engine on a whole

#pragma once

#include "cmnPriv.h"
#include "Window.h"
#include "Dimension.h"
#include "DimensionFactory.h"
#include "valid.h"

/*
#include "SenseFactory.h"
#include "Sense.h"
 */

class IncidentMainWindow : public Window, public valid {
public:
    IncidentMainWindow();
    IncidentMainWindow(WindowFactory &wf, DimensionFactory &df/*, SenseFactory &sf*/);

    ~IncidentMainWindow();

    RESULT BuildWindow(WindowFactory &factory);
    RESULT BuildDimensionImplementation(DimensionFactory &factory);

    // Also need window imp for this
    //RESULT BuildSense(SenseFactory &factory); //, WindowImp *pWindowImp);
    RESULT StartMain(RESULT (*F[])(void*), int Functions_c);

    // Dimension Manipulation functions
    RESULT AddObject(DimObject *obj, DimObject * &pObj) {
        return m_dimension->AddObject(obj, pObj);
    }

    RESULT AddObject(FlatObject *obj, FlatObject* &pObj) {
        return m_dimension->AddObject(obj, pObj);
    }
    
    /*
    // Sense Callbacks
    // Since SENSE should be able to handle all device types eventually
    // we need to pass the device type as well as the device ID.  The
    // index refers to the index of the specific input registered for
    RESULT RegisterSenseInput(SENSE_DEVICE_TYPE DeviceType, int DeviceID, int index, SenseCallback Callback, void *pContext) {
        RESULT r = R_OK;
        CRM(m_sense->RegisterInput(DeviceType, DeviceID, index, Callback, pContext), "IncidentMainWindow: Faield to register indexed callback");

Error:
        return r;
    }

    RESULT RegisterDefaultSenseInput(SENSE_DEVICE_TYPE DeviceType, int DeviceID, SenseCallback Callback, void *pContext)
    {
        RESULT r = R_OK;
        CRM(m_sense->RegisterDefaultInput(DeviceType, DeviceID, Callback, pContext), "IncidentMainWindow: Faield to register indexed callback");
Error:
        return r;
    }
     */

    int GetWinWidth(){ return *((int*)(m_pWindowImp->getParam(WINDOW_WIDTH))); }
    int GetWinHeight(){ return *((int*)(m_pWindowImp->getParam(WINDOW_HEIGHT))); }

    int GetBufferWidth(){ return m_dimension->GetDimensionImpWidth(); }
    int GetBufferHeight(){ return m_dimension->GetDimensionImpHeight(); }
    
private:

    // Dimension graphics engine instance
    Dimension *m_dimension;

    // Sense Input State
    //Sense *m_sense;
};