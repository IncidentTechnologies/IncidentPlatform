//
//  FuzzExpDistortionNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_FuzzExpDistortion_h
#define gTarAudioController_FuzzExpDistortion_h

#include "EffectNode.h"
//#include "LocalMax.h"
#include <math.h>

class FuzzExpDistortionNode : public EffectNode
{
    
public:
    FuzzExpDistortionNode(double gain, double wet, double SamplingFrequency);
    
    double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    bool SetGain(double gain);
    
private:
    double m_gain;
    double m_runningAvg;
    
};

#endif

