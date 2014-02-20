// This is the base class for the DimensionTranslators which will take a given
// DimObject and spit out the implementation specific objects
// Dimension Object -> Translator -> Implementation Specific Object

#pragma once

#include "DimObject.h"

class DimensionTranslator
{
public:

    // This function will take an input object and spit out a new object
    // the previous object is thereby deleted within the function
    virtual RESULT TranslateObject(DimObject * &d_doIn, DimObject * &n_doOut) = 0;

};