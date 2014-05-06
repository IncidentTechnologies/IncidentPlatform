//
//  ChorusEffectNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_ChorusEffect_h
#define gTarAudioController_ChorusEffect_h

#include "EffectNode.h"
#include <math.h>

#define CHORUS_EFFECT_MAX_MS_DELAY 500

// Main difference between a chorus and delay is that a chrous does not feedback
// and only plays the input signal again with a delay defined in the chorus
class ChorusEffectNode : public EffectNode
{
public:
    ChorusEffectNode(double msDelayTime, double depth, double width, double LFOFreq, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    void Reset();
    void ClearOutEffect();
    
    bool SetMSDelayTime(double msDelayTime);
    bool SetDepth(double depth);
    bool SetLFOFreq(double freq);
    bool SetWidth(double width);
    virtual bool setPrimaryParam(float value);
    virtual bool setSecondaryParam(float value);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    
    ~ChorusEffectNode();
    
private:
    double m_SamplingFrequency;
    Parameter *m_pDelayTime;
    
    long m_pDelayLine_n;
    double *m_pDelayLine;
    
    
    long m_pDelayLine_c;
    long m_pDelayLine_l;
    
    Parameter *m_pDepth;
    Parameter *m_pWidth;
    Parameter *m_pLFOFreq;
};

#endif
