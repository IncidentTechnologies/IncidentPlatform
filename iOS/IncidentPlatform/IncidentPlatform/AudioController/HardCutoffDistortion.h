//
//  HardCutoffDistortion.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//
//  Hard cutoff with overdrive. Hard limit the input then scale it up to
//  have the same amplitue as the undistorted input.

#ifndef gTarAudioController_HardCutoffDistortion_h
#define gTarAudioController_HardCutoffDistortion_h

#include "Effect.h"
#include "LocalMax.h"


class HardCutoffDistortion :
public Effect
{
public:
    HardCutoffDistortion (double cutoff, double wet, double SamplingFrequency);
    bool SetCutoff(double cutoff);
    inline double InputSample(double sample);
    ~HardCutoffDistortion();
    
private:
    double m_cutoff;
    LocalMax *m_pLocalMax;
};
#endif
