//
//  GtarSamplerNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 5/13/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__GtarSamplerNode__
#define __IncidentPlatform__GtarSamplerNode__

#include "AudioNodeNetwork.h"
#include "dss_list.h"
#include "SampleNode.h"
#include "SamplerNode.h"

#define MAX_BANKS 5
#define MAX_SAMPLES 102

class SampleNode;

using namespace dss;

// The Gtar Sample Buffer extends the regular Sample Buffer with
// added envelope functionality

class GtarSampleBuffer : public SampleBuffer {
public:
    
    GtarSampleBuffer(char *pszFilename);
    ~GtarSampleBuffer();
    
    float GtarGetNextSample(unsigned long int timestamp);
    RESULT GtarResetSampleCounter();
    bool GtarSampleDone();
    bool GtarSamplePlaying();
    RESULT GtarStartPlaying();
    RESULT GtarSampleInterrupt();
    
    void NoteOn();
    void NoteMutedOn();
    void NoteSlideOn();
    void NoteOff();
    bool IsNoteOn();
    
private:
    float EnvelopeSample(float retVal);
    
private:
    int m_channel_n;
    float m_msCLKIncrement;
    float m_releaseCLK;
    bool m_fNoteOn;
    
public:
    float m_CLK;
    
    float m_msAttack;
    float m_AttackLevel;
    
    float m_msDecay;
    float m_SustainLevel;
    
    float m_msRelease;
    float m_ReleaseLevel;
    
    float m_releaseScaleFactor;
};

// A sampler is a collection of Sample nodes mixed into
// a bank, and all the banks are mixed into the output
// node.

class GtarSamplerNode : public GeneratorNode {
public:
    GtarSamplerNode();
    ~GtarSamplerNode();
    
    float GetNextSample(unsigned long int timestamp);
    
    RESULT ReleaseBank(int bank);
    int CreateNewBank(int bank, int numSamples);
    
    RESULT TriggerSample(int bank, int sample);
    RESULT TriggerMutedSample(int bank, int sample);
    unsigned long int TriggerContinuousSample(int bank, int sampleLead, int sampleTrail);
    
    RESULT StopSample(int bank, int sample);
    RESULT NoteOff(int bank, int sample);
    bool IsNoteOn(int bank, int sample);
    
    RESULT LoadSampleIntoBank(int bank, char *pszFilepath);
    
public:
    GtarSampleBuffer *m_buffers[MAX_BANKS][MAX_SAMPLES];
    
    bool m_fPlaying;
    
    int m_maxBank;
    int m_numSamples[MAX_BANKS];
    int m_nextSampleCounter[MAX_BANKS];
    
    int m_sampleTransitionIndex[MAX_BANKS][MAX_SAMPLES];
    unsigned long int m_sampleBufferTransitionIndex[MAX_BANKS][MAX_SAMPLES];
    
};


#endif /* defined(__IncidentPlatform__SamplerNode__) */
