//
//  FuzzExpDistortion.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_FuzzExpDistortion_h
#define gTarAudioController_FuzzExpDistortion_h

#include "Effect.h"
#include "LocalMax.h"

class FuzzExpDistortion : public Effect
{

public:
    FuzzExpDistortion(double gain, double wet, double SamplingFrequency);

    inline double InputSample(double sample);
    bool SetGain(double gain);
    
private:
    double m_gain;
    double m_runningAvg;

};

#endif
