//
//  DelayEffect.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_DelayEffect_h
#define gTarAudioController_DelayEffect_h

#include "EffectNode.h"

#define DELAY_EFFECT_MAX_MS_DELAY 1000

class DelayNode : public EffectNode {
public:
    DelayNode(double msDelayTime, double feedback, double wet);
    ~DelayNode();
    
    float InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    float GetFeedback();
    bool SetFeedback(float feedbackValue);
    
    float GetDelayTime();
    bool SetDelayTime(float delayTime);
    
    void Reset();
    void ClearOutEffect();
    
    bool setPrimaryParam(float value);
    bool setSecondaryParam(float value);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    
protected:
    Parameter *m_pDelayTime;
    Parameter *m_pFeedback;
    
    long m_pDelayLine_n;
    double *m_pDelayLine;
    long m_pDelayLine_c;
    long m_pDelayLine_l;
};


#endif
