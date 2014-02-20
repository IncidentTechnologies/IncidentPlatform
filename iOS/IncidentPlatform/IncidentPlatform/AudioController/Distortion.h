//
//  Distortion.h
//  gTarAudioController
//
//  Created by Franco Cedano on 1/24/12.
//  Copyright (c) 2012 Incident Technologies. All rights reserved.
//
//  

#ifndef gTarAudioController_Distortion_h
#define gTarAudioController_Distortion_h

#include "Effect.h"
#import "TanhDistortion.h"
#include "FuzzExpDistortion.h"

static const float defaultDrive = 3.0;
static const float defaultFuzz = 3.0;

class Distortion : public Effect
{
    
public:
    Distortion (double gain, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    
    Parameter& getPrimaryParam();
    
    // This param is the negDistortion param of the TanhDistortion unit. Here we
    // will also set the posDistortion param based on the neg value.
    bool setPrimaryParam(float value);
    
    Parameter& getSecondaryParam();
    
    bool setSecondaryParam(float value);
    
    void Reset();
    
    ~Distortion();

 
private:
    TanhDistortion *m_pTanhDistortion;
    FuzzExpDistortion *m_pFuzzExpDistortion;
    
    Parameter *m_pDrive;
    Parameter *m_pFuzzGain;
    float m_gain;
};

#endif
