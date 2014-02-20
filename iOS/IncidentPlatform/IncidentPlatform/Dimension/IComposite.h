#pragma once

// Common Control Interface
// Defines the interface which all common control must contain

#include "RESULT.h"
#include "EHM.h"
#include "DimensionTranslator.h"
#include "FlatTranslator.h"
#include "DimensionImp.h"

/*interface*/
class IComposite {
public:
    // Returns the type of the common control
    // virtual CommonControlType GetType() = 0;

    // Translate Control is a method the control must implement
    // which will translate all of the Flat objects inside of the
    // controller to the appropriate implementation specific 
    // objects.  As such the control is not aware of this and 
    // remains generic
    virtual RESULT Translate(FlatTranslator *pFlatTranslator) = 0;
    virtual RESULT Translate(DimensionTranslator *pDimTranslator) = 0;

    virtual RESULT Initialize(DimensionImp *pDimImp) = 0;
    virtual RESULT Render(DimensionImp *pDimImp) = 0;
    virtual RESULT Update() = 0;
};