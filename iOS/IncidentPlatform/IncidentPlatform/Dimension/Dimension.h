// Dimension is the container for the graphics
// object store as well as the implementation
// This is the main object for the graphics engine and object store

#pragma once

#include <stdlib.h>

#include "DimensionImp.h"
#include "ObjectStore.h"

class DimensionImp;

class Dimension
{
public:
    Dimension(DimensionImp *pdimensionImp)
    { 
        RESULT res = R_SUCCESS;
        
        m_dimensionImp = pdimensionImp;                
        
        m_ObjectStore = new ObjectStore(m_dimensionImp->GetDimensionType(), m_dimensionImp);
    }

    ~Dimension()
    {         
        delete m_ObjectStore;
    }

    RESULT InitializeDimension(WindowImp &windowImp)
    {
        RESULT r = R_SUCCESS;
        
        CRM(m_ObjectStore->GetDimensionObjectDataBase()->Balance(), "Failed to balance object database");
        CRM(m_dimensionImp->Initialize(windowImp, m_ObjectStore), "Failed to Initialize dimension implementation");

Error:
        return r;
    }

    RESULT RenderDimension()
    {
        return m_dimensionImp->Render(m_ObjectStore);
    }   

    // Object Store Manipulation
    RESULT AddObject(DimObject *d_obj, DimObject * &pObj)
    {
        return m_ObjectStore->AddObject(d_obj, pObj);
    }

    RESULT AddObject(FlatObject *d_obj, FlatObject* &pObj)
    {
        return m_ObjectStore->AddObject(d_obj, pObj);
    }

    int GetDimensionImpWidth(){ return m_dimensionImp->GetWidth(); }
    int GetDimensionImpHeight(){ return m_dimensionImp->GetHeight(); }


    /* LOOK: Do not support delete for now
    RESULT DeleteObject(DimObject *d_obj)
    {
        return m_ObjectStore->DeleteObject(d_obj);
    }
    */

    // Some Fun Test Code
    // **************************************************
    static RESULT RotateObjects(DimObject *pObj)
    {
        dynamic_cast<DimObject *>(pObj)->RotateX((float)(rand() % 100)/(float)1000);
        dynamic_cast<DimObject *>(pObj)->RotateY((float)(rand() % 100)/(float)1000);
        dynamic_cast<DimObject *>(pObj)->RotateZ((float)(rand() % 100)/(float)1000);

        return R_SUCCESS;
    }
    
    RESULT RotateObjects()
    {
        m_ObjectStore->GetDimensionObjectDataBase()->Iterate(RotateObjects);       
        return R_SUCCESS;
    }
    // ***************************************************
    
public:
    DimensionImp *m_dimensionImp;
    ObjectStore *m_ObjectStore;
};