//
//  HardCutoffDistortionNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_HardCutoffDistortion_h
#define gTarAudioController_HardCutoffDistortion_h

#include "EffectNode.h"
#include "LocalMax.h"


class HardCutoffDistortionNode :
public EffectNode
{
public:
    HardCutoffDistortionNode (double cutoff, double wet, double SamplingFrequency);
    
    bool SetCutoff(double cutoff);
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    ~HardCutoffDistortionNode();
    
private:
    double m_cutoff;
    LocalMax *m_pLocalMax;
};
#endif
