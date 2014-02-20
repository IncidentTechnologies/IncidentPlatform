//
//  FirstOrderFilter.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_FirstOrderFilter_h
#define gTarAudioController_FirstOrderFilter_h

#include "Effect.h"

/*
 A simple filter that feeds back the previous sample times
 a feedback factor and adds it to the current sample.
 */
class FirstOrderFilter :
public Effect
{
public:
    FirstOrderFilter(double feedback, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    bool SetFeedback(double feedback);
    
private:
    double m_previousSample;
    double m_feedback;
};

#endif
