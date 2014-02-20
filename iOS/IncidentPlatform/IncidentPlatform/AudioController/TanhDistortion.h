//
//  TanhDistortion.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//
//  Distortion effect, applies the input signal to a hyperbolic tangent
//  function.
//

#ifndef gTarAudioController_TanhDistortion_h
#define gTarAudioController_TanhDistortion_h

#include "Effect.h"
#include "Parameter.h"

class TanhDistortion : public Effect
{
public:
    TanhDistortion (double gain, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    
    bool setPosFactor(double factor);
    
    bool setNegFactor(double factor);
    
    Parameter& getPrimaryParam();
    
    bool setPrimaryParam(float value);
    
    Parameter& getSecondaryParam();
    
    bool setSecondaryParam(float value);
    
    ~TanhDistortion();
    
private:
    Parameter *m_pPosFactor;
    Parameter *m_pNegFactor;
};


#endif
