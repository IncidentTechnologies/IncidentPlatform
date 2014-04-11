//
//  ButterWorthFilterElementNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_ButterWorthFilterElement_h
#define gTarAudioController_ButterWorthFilterElement_h

#include "EffectNode.h"
#include <math.h>

class ButterWorthFilterElementNode : public EffectNode {
public:
    
    // This will calculate the coefficients for the given order, k, cutoff, and sampling frequency provided
    bool CalculateCoefficients(int k, float cutoff, float SamplingFrequency);
    
    ButterWorthFilterElementNode(int k, int order, float cutoff, float SamplingFrequency);
    
    ~ButterWorthFilterElementNode();
    
    // Simplification of the InputSamples function
    // creates a delay line of the order of the filter
    // this will return 0 until the line is filled
    /*inline */
    double InputSample(double sample);
    //float GetNextSample(unsigned long int timestamp);
    
    void Reset();
    
    void ClearOutEffect();
    
private:
    /*inline */double InputSamples(double *pSamples, int pSamples_n);
    
private:
    
    //NSLock *lock;
    bool lock;
    int m_order;
    int m_k;
    double m_cutoff;
    double m_SamplingFrequency;
    
    double *m_pDelayLine;
    int m_pDelayLine_n;
    
    double *m_pAC;
    int m_pAC_n;
    
    double *m_pBC;
    int m_pBC_n;
    
    double *m_pSampleDelay;
    int m_pSampleDelay_n;
    int m_pSampleDelayCount;
    
    bool m_fReady;
};

#endif