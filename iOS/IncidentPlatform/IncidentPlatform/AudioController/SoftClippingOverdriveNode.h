//
//  SoftClippingOverdriveNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_SoftClipingOverdrive_h
#define gTarAudioController_SoftClipingOverdrive_h

#include "EffectNode.h"
#include "LocalMax.h"
#include <math.h>

class SoftClippingOverdriveNode :
public EffectNode
{
public:
    SoftClippingOverdriveNode (double threshold,double multiplier, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    bool SetThreshold(double threshold);
    bool SetMultiplier(double multiplier);
    
    ~SoftClippingOverdriveNode();
    
private:
    double m_threshold;
    double m_multiplier;
    LocalMax *m_pLocalMax;
};

#endif