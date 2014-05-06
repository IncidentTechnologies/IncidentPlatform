//
//  TanhDistortionNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_TanhDistortion_h
#define gTarAudioController_TanhDistortion_h

#include "EffectNode.h"
#include "Parameter.h"
#include <math.h>

class TanhDistortionNode : public EffectNode
{
public:
    TanhDistortionNode (double gain, double wet, double SamplingFrequency);
    
    double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    bool setPosFactor(double factor);
    bool setNegFactor(double factor);
    
    bool setPrimaryParam(float value);
    bool setSecondaryParam(float value);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    
    
    ~TanhDistortionNode();
    
private:
    Parameter *m_pPosFactor;
    Parameter *m_pNegFactor;
};


#endif