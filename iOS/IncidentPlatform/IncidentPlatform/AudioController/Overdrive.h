//
//  Overdrive.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//
//  Simple overdrive effect. Multiplies the input signal by a gain factor.

#ifndef gTarAudioController_Overdrive_h
#define gTarAudioController_Overdrive_h

#include "Effect.h"

class Overdrive :
public Effect
{
public:
    Overdrive(double gain, double wet, double SamplingFrequency);
    
    bool SetGain(double gain);
    
    inline double InputSample(double sample);
    
    ~Overdrive();
    
private:
    double m_gain;
};

#endif
