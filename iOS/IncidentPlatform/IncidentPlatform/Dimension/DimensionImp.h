// Dimension Implementation 

#pragma once

#include "WindowImp.h"
//#include "ObjectStore.h"
#include "DimCmn.h"

class ObjectStore;

class DimensionImp
{
public:
    virtual RESULT Initialize(WindowImp &windowImp, ObjectStore *objStore) = 0;

    virtual RESULT Render(ObjectStore *objStore) = 0;

    DIMENSION_TYPE GetDimensionType(){ return m_DimType; }

    // Usually used for information regarding
    // the screen buffer
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;

protected:
    DIMENSION_TYPE m_DimType;
};