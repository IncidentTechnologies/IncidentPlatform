#ifdef __WIN32__
// This is the DirectX9 Translator which will take normal DimObjects and 
// translate them into their DirectX9 counterparts

// Should also handle Flat objects for this particular implementation

#pragma once

#include "DimensionTranslator.h"
#include "FlatTranslator.h"
#include "DX9Primitives.h"
#include "IComposite.h"

class DX9Translator :
    public DimensionTranslator,
    public FlatTranslator
{

public:
    RESULT TranslateObject(FlatObject* &d_foIn, FlatObject* &n_foOut);
    RESULT TranslateObject(DimObject * &d_doIn, DimObject * &n_doOut);
};

#endif // #ifdef __WIN32__
