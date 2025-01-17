//
// SampleNode.h
// IncidentPlatform
//
// Created by Idan Beck on 2/17/14.
// Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__SampleNode__
#define __IncidentPlatform__SampleNode__

#include <iostream>
#include "GeneratorNode.h"
#include <AudioToolbox/AudioToolbox.h>

// The sample buffer actually contains the sample buffer
class SampleBuffer {
public:
    SampleBuffer(char *pszFilename);
    SampleBuffer(void *buffer, unsigned long int bufferLength);
    ~SampleBuffer();
    
    RESULT LoadSampleBufferFromPath(char *pszPath);
    RESULT LoadSampleBufferFromString(const void *pszBuffer, unsigned long int bufferLength);
    inline float GetNextSample(unsigned long int timestamp);
    RESULT ResetSampleCounter();
    inline bool SampleDone();
    
    inline RESULT NormalizeSample();
    
    float GetSampleBufferLengthMS();
    RESULT SetStart(float msStart);
    RESULT SetEnd(float msEnd);
    float GetStart();
    float GetEnd();
    
    RESULT SetTime(float msTime);
    
    int GetSampleRate();
    
    unsigned long int GetByteSize();
    void *GetBufferArray();
    
public:
    RESULT SetSampleRate(unsigned long samplerate);
    
private:
    RESULT SetStartSample(unsigned long start);
    RESULT SetEndSample(unsigned long end);
    RESULT SetSample(unsigned long sample);
    unsigned long GetSampleCount();
    unsigned long GetStartStopSampleCount();
    
private:
    AudioStreamBasicDescription GetClientFormatDescription(bool fStereo);
    
public:
    RESULT SaveToFile(char *pszFilepath, bool fOverwrite);
    
private:
    int m_SampleRate;
public:
    unsigned long int m_pBuffer_c;
    float *m_pBuffer;
    unsigned long int m_pBuffer_n;
    
    unsigned long int m_pBuffer_start;
    unsigned long int m_pBuffer_end;
    
    float m_normalScale;
    
};


// A sample node consists of a sample buffer and manipulation functions to play back
class SampleNode : public GeneratorNode {
public:
    SampleNode(int xmpid);
    SampleNode(char *pszFilename);
    SampleNode(void *buffer, unsigned long int bufferLength);
    ~SampleNode();
    
    float GetNextSample(unsigned long int timestamp);
    
    RESULT SaveToFile(char *pszFilepath, bool fOverwrite);
    
    RESULT Trigger();
    RESULT Stop();
    RESULT Resume();
    RESULT SetTime(float msTime);
    
    // All these are in ms
    float GetLength();
    RESULT SetStart(float msStart);
    RESULT SetEnd(float msEnd);
    
    SampleBuffer *GetSampleBuffer();
    
public:
    SampleBuffer *m_pSampleBuffer;
    bool m_fPlaying;
};


#endif /* defined(__IncidentPlatform__SampleNode__) */