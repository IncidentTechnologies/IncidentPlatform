//
//  FirstOrderFilter.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_FirstOrderFilter_h
#define gTarAudioController_FirstOrderFilter_h

#include "EffectNode.h"

/*
 A simple filter that feeds back the previous sample times
 a feedback factor and adds it to the current sample.
 */
class FirstOrderFilterNode : public EffectNode {
public:
    FirstOrderFilterNode(double feedback, double wet);
    
    double InputSample(double sample);
    bool SetFeedback(double feedback);
    
    float GetNextSample(unsigned long int timestamp);
    
private:
    double m_previousSample;
    double m_feedback;
};

#endif
