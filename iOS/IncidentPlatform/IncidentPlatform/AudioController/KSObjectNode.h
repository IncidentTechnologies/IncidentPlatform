//
//  KSObjectNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef gTarAudioController_KSObject_h
#define gTarAudioController_KSObject_h

#include "ButterWorthFilterNode.h"
#include <math.h>

#define MIN_FREQ 50

class KSObjectNode {
public:
	KSObjectNode();
	
	KSObjectNode(float SamplingFreq);
	
    /*
     freq - freqency of note to pluck
     pluckAmplitude - factor indicating how strongly the string was plucked, in range [0,1]
     */
	void Pluck(float freq, float pluckAmplitude);
    
    bool SetBWFilterCutoff(double cutoff);
    
    bool SetBWFilterOrder(int order);
	
	inline double GetNextKSSample();
    
    void Set3rdOrderHarmonicOn(bool on);
    
    void Set5thOrderHarmonicOn(bool on);
    
    bool SetNoiseScale(float scale);
    
    bool SetNoiseVariation(float variation);
    
    void SetSawToothOn(bool on);
    
    bool SetSawMultiplier(float multiplier);
    
    void SetSqWaveOn(bool on);
    
    bool SetSqMultiplier(float multiplier);
    
    bool SetAttenuationVariation(float variation);
	
	~KSObjectNode();
	
private:
	float *m_BufferKS;
	int m_BufferKS_n;
    int m_BufferKS_CurLength;
	int m_eBufferKS;
	float m_FreqKS;
    
    ButterWorthFilterNode *m_pBWFilter;
    int m_bwFltOrder;
    double m_bwFltCutoff;
    
    bool m_b3rdHarmonicOn;
    bool m_b5thHarmonicOn;
    bool m_bSawToothOn;
    int m_sawFreqMultiplier;
    bool m_bSqOn;
    int m_sqFreqMultiplier;
    float m_noiseScale;
    // variation as a percentage of the main value that we randomly deviate from
    float m_noiseVariation;
    float m_currentAttenuation;
    float m_attenuationVariation;
    
public:
	float m_Fs;
	
public:
	float m_attenuationKS;
	
public:
	static float GuitarFreqLookup(int string, int fret);
};

#endif
