//
//  ButterWorthFilterNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_ButterWorthFilter_h
#define gTarAudioController_ButterWorthFilter_h

#include "ButterWorthFilterElementNode.h"
#include "EffectNode.h"
#include "Parameter.h"

#define MAX_BUTTERWORTH_ORDER 16

class ButterWorthFilterNode: public EffectNode
{
public:
    ButterWorthFilterNode(int order, double cutoff, double SamplingFrequency);
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    bool SetCutoff(double cutoff);
    bool SetOrder(int order);
    
    void Reset();
    
    bool setPrimaryParam(float value);
    bool setSecondaryParam(float value);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    
    ~ButterWorthFilterNode();
    
private:
    ButterWorthFilterElementNode **m_ppFilters;
    int m_ppFilters_n;
    int m_order;
    Parameter *m_pCutoff;                             // cutoff frequency in HZ (elements convert to radians)
    double m_SamplingFrequency;
    
};


#endif
