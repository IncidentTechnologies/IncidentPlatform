//
// GtarSamplerNode.h
// IncidentPlatform
//
// Created by Kate Schnippering on 5/13/14.
// Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__GtarSamplerNode__
#define __IncidentPlatform__GtarSamplerNode__

#include "AudioNodeNetwork.h"
#include "dss_list.h"
#include "SampleNode.h"
#include "SamplerNode.h"
#include "Parameter.h"

#define MAX_BANKS 2
#define MAX_SAMPLES 127

class SampleNode;

using namespace dss;

// The Gtar Sample Buffer extends the regular Sample Buffer with
// added envelope functionality

class GtarSampleBuffer : public SampleBuffer {
public:
    
    GtarSampleBuffer(char *pszFilename);
    ~GtarSampleBuffer();
    
    inline float GtarGetNextSample(unsigned long int timestamp);
    inline bool GtarSampleDone();
    inline bool GtarSamplePlaying();
    inline RESULT GtarStartPlaying();
    inline RESULT GtarSampleInterrupt();
    
    inline RESULT NormalizeSample();
    
    void NoteOn();
    void NoteMutedOn();
    void NoteSlideOn();
    void NoteOff();
    void StopNote();
    bool IsNoteOn();
    
private:
    inline float EnvelopeSample(float retVal);
    
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
    
    float m_normalScale;
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
    RESULT StopNote(int bank, int sample);
    bool IsNoteOn(int bank, int sample);
    
    RESULT LoadSampleIntoBank(int bank, char *pszFilepath);
    RESULT LoadSampleIntoBankAtIndex(int bank, int index, char *pszFilepath);
    
    virtual bool setPrimaryParam(float value);
    virtual bool setSecondaryParam(float value);
    
    Parameter* getPrimaryParam();
    Parameter* getSecondaryParam();
    
public:
    GtarSampleBuffer *m_buffers[MAX_BANKS][MAX_SAMPLES] = {{NULL}};
    
    bool m_fPlaying;
    
    int m_maxBank;
    int m_numSamples[MAX_BANKS];
    int m_nextSampleCounter[MAX_BANKS];
    
    int m_sampleTransitionIndex[MAX_BANKS][MAX_SAMPLES];
    unsigned long int m_sampleBufferTransitionIndex[MAX_BANKS][MAX_SAMPLES];
    
    Parameter *m_pRewind;
    Parameter *m_pTransition;
    
};


#endif /* defined(__IncidentPlatform__SamplerNode__) */