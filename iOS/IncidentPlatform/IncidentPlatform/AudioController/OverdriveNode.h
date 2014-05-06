//
//  OverdriveNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_Overdrive_h
#define gTarAudioController_Overdrive_h

#include "EffectNode.h"

class OverdriveNode :
public EffectNode
{
public:
    OverdriveNode(double gain, double wet, double SamplingFrequency);
    
    bool SetGain(double gain);
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    ~OverdriveNode();
    
private:
    double m_gain;
};

#endif
