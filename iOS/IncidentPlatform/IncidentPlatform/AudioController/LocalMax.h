//
//  LocalMax.h
//  gTarAudioController
//
//  Created by Franco Cedano on 12/7/11.
//  Copyright (c) 2011 Incident Technologies. All rights reserved.
//  Keeps track of the local maximum of the absolute value of the signal over
//  a sample window equivalent to 2 wavelengths of the lowest frequency
//  produced (open low E string, 82.407Hz)

#ifndef gTarAudioController_LocalMax_h
#define gTarAudioController_LocalMax_h

class LocalMax
{
public:
    LocalMax(double SamplingFrequency);
    
    double GetLocalMax(double sample);
    
    ~LocalMax();
    
private:
    double m_SamplingFrequency;
    double *m_pSampleBuffer;
    int m_buffLen;
    int m_currentSampleIndex;
    float m_currentMax;
    int m_currentMaxAge;
    
};


#endif
