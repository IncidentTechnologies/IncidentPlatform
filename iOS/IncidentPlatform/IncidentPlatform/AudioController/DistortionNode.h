//
//  DistortionNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_Distortion_h
#define gTarAudioController_Distortion_h

#include "EffectNode.h"
#import "TanhDistortionNode.h"
#include "FuzzExpDistortionNode.h"

static const float defaultDrive = 3.0;
static const float defaultFuzz = 3.0;

class DistortionNode : public EffectNode
{
    
public:
    DistortionNode (double gain, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);

    bool setPrimaryParam(float value);
    bool setSecondaryParam(float value);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    
    void Reset();
    // void ClearOutEffect();
    
    ~DistortionNode();
    
// protected:
private:
    TanhDistortionNode *m_pTanhDistortion;
    FuzzExpDistortionNode *m_pFuzzExpDistortion;
    
    Parameter *m_pDrive;
    Parameter *m_pFuzzGain;
    float m_gain;
};

#endif
