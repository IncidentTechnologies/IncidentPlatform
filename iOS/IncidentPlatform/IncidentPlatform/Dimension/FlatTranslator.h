#pragma once

// This is the base class for the FlatTranslator which takes a 
// particular flat object and will spit out the implementation
// specific object

#include "FlatObject.h"

class FlatTranslator
{
public:
    // Function will delete input object and allocate memory for the new object
    virtual RESULT TranslateObject(FlatObject* &d_foIn, FlatObject* &n_foOut) = 0;
};