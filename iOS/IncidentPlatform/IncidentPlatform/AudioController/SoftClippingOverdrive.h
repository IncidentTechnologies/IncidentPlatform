//
//  SoftClipingOverdrive.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_SoftClipingOverdrive_h
#define gTarAudioController_SoftClipingOverdrive_h

#include "Effect.h"
#include "LocalMax.h"

class SoftClippingOverdrive :
public Effect
{
public:
    SoftClippingOverdrive (double threshold,double multiplier, double wet, double SamplingFrequency);
    
    inline double InputSample(double sample);
    bool SetThreshold(double threshold);
    bool SetMultiplier(double multiplier);
    
    ~SoftClippingOverdrive();
    
private:
    double m_threshold;
    double m_multiplier;
    LocalMax *m_pLocalMax;
};

#endif
