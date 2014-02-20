// Dimension Factory Class
// This object will create a Dimension object with the correct type
// of graphics API

#pragma once

#include "DimensionImp.h"
#include "DimensionImpDX9.h"

class DimensionFactory {
public:
    DimensionFactory() {
        m_dimensionType = DIMENSION_DEFAULT;
    }

    DimensionFactory(DIMENSION_TYPE dt) {
        m_dimensionType = dt;
    }

    ~DimensionFactory(){ };

    virtual DimensionImp* MakeDimension(DIMENSION_TYPE dt, WindowImp *pWindowImp) const {
        switch(dt) {
#ifdef __WIN32__
            case DIMENSION_DX9: {
                return new DimensionImpDX9(dt, pWindowImp);
            } break;
#endif
        
            default: {
                return NULL;
            } break;
        }
    }

    virtual DimensionImp* MakeDimension(WindowImp *pWindowImp) const {
        return this->MakeDimension(m_dimensionType, pWindowImp);
    }

private:
    DIMENSION_TYPE m_dimensionType;
};