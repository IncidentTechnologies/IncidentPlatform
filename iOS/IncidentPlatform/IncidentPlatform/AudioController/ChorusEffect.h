//
//  ChorusEffect.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_ChorusEffect_h
#define gTarAudioController_ChorusEffect_h

#include "Effect.h"

#define CHORUS_EFFECT_MAX_MS_DELAY 500

// Main difference between a chorus and delay is that a chrous does not feedback
// and only plays the input signal again with a delay defined in the chorus
class ChorusEffect : public Effect
{
public:
    ChorusEffect(double msDelayTime, double depth, double width, double LFOFreq, double wet, double SamplingFrequency);
    
    bool SetMSDelayTime(double msDelayTime);
    bool SetDepth(double depth);
    bool SetLFOFreq(double freq);
    bool SetWidth(double width);
    
    inline double InputSample(double sample);
    
    void Reset();
    
    void ClearOutEffect();
    
    Parameter& getPrimaryParam();
    
    virtual bool setPrimaryParam(float value);
    
    Parameter& getSecondaryParam();
    
    virtual bool setSecondaryParam(float value);
    
    ~ChorusEffect();
    
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
