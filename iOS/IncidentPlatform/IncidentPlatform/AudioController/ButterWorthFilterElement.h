//
//  ButterWorthFilterElement.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//

#ifndef gTarAudioController_ButterWorthFilterElement_h
#define gTarAudioController_ButterWorthFilterElement_h

class ButterWorthFilterElement {
public:
    
    // This will calculate the coefficients for the given order, k, cutoff, and sampling frequency provided
    bool CalculateCoefficients(int k, float cutoff, float SamplingFrequency);
    
    ButterWorthFilterElement(int k, int order, float cutoff, float SamplingFrequency);
    
    ~ButterWorthFilterElement();
    
    // Simplification of the InputSamples function
    // creates a delay line of the order of the filter
    // this will return 0 until the line is filled
    /*inline */double InputSample(double sample);
    
    void Reset();
    
    void ClearOutEffect();
    
private:
    /*inline */double InputSamples(double *pSamples, int pSamples_n);
    
private:
    
    NSLock *lock;
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
