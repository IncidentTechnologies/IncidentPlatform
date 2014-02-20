//
//  ButterWorthFilter.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_ButterWorthFilter_h
#define gTarAudioController_ButterWorthFilter_h

#include "ButterWorthFilterElement.h"
#include "Effect.h"
#include "Parameter.h"

#define MAX_BUTTERWORTH_ORDER 16

class ButterWorthFilter : public Effect
{
public:
    ButterWorthFilter(int order, double cutoff, double SamplingFrequency);
    
    bool SetCutoff(double cutoff);
    inline double InputSample(double sample);
    bool SetOrder(int order);
    void Reset();
    Parameter& getPrimaryParam();
    bool setPrimaryParam(float value);
    Parameter& getSecondaryParam();
    bool setSecondaryParam(float value);
    ~ButterWorthFilter();
    
private:
    ButterWorthFilterElement **m_ppFilters;
    int m_ppFilters_n;
    int m_order;
    Parameter *m_pCutoff;                             // cutoff frequency in HZ (elements convert to radians)
    double m_SamplingFrequency;
    
};


#endif
