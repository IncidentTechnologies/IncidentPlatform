//
// GtarSamplerNode.cpp
// IncidentPlatform
//
// Created by Kate Schnippering on 5/13/14.
// Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "GtarSamplerNode.h"
#include "SampleNode.h"
#include "CAXException.h"
#include <AudioToolbox/AudioFormat.h>

#define DEFAULT_MSATTACK 10.0f
#define DEFAULT_ATTACKLEVEL 1.0f
#define DEFAULT_MSDECAY 100.0f
#define DEFAULT_SUSTAINLEVEL 1.0f
#define DEFAULT_MSRELEASE 500.0f

#define MUTED_MSATTACK 10.0f
#define MUTED_ATTACKLEVEL 0.3f
#define MUTED_SUSTAINLEVEL 0.0f
#define MUTED_MSDECAY 200.0f

#define SLIDE_MSATTACK 0.0f
#define SLIDE_ATTACKLEVEL 0.0f

/*************************************/
// GtarSampleBuffer
/*************************************/

// Envelope control for Gtar
GtarSampleBuffer::GtarSampleBuffer(char *pszFilenamePath) :
SampleBuffer(pszFilenamePath),
m_msAttack(DEFAULT_MSATTACK),
m_AttackLevel(DEFAULT_ATTACKLEVEL),
m_msDecay(DEFAULT_MSDECAY),
m_SustainLevel(DEFAULT_SUSTAINLEVEL),
m_msRelease(DEFAULT_MSRELEASE),
m_CLK(0.0f),
m_releaseCLK(0.0f)
{
    m_fNoteOn = false;
    m_msCLKIncrement = 1000.0f / DEFAULT_SAMPLE_RATE;
}

GtarSampleBuffer::~GtarSampleBuffer() {
    if(m_pBuffer != NULL) {
        free(m_pBuffer);
        m_pBuffer = NULL;
    }
}

inline bool GtarSampleBuffer::GtarSampleDone() {
    
    if(m_pBuffer_c >= m_pBuffer_end){
        m_fNoteOn = false;
    }
    
    return (m_pBuffer_c >= m_pBuffer_end);
}

inline RESULT GtarSampleBuffer::GtarSampleInterrupt() {
    ResetSampleCounter();
    m_fNoteOn = false;
    
    return R_SUCCESS;
}

inline RESULT GtarSampleBuffer::GtarStartPlaying() {
    m_pBuffer_c++;
    m_fNoteOn = true;
    return R_SUCCESS;
}

inline bool GtarSampleBuffer::GtarSamplePlaying() {
    
    return (m_pBuffer_c > m_pBuffer_start && m_pBuffer_c < m_pBuffer_end);
}

inline float GtarSampleBuffer::GtarGetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_pBuffer != NULL && m_pBuffer_n > 0 && m_pBuffer_end > 0 && m_pBuffer_c < m_pBuffer_end) {
        retVal = m_pBuffer[m_pBuffer_c];
        
        if(m_pBuffer_c > 0){
            m_pBuffer_c++;
        }
    }
    
    return EnvelopeSample(retVal);
}

inline float GtarSampleBuffer::EnvelopeSample(float retVal){
    
    // Enveloping
    float scaleFactor = 0.0f;
    
    if(m_fNoteOn) {
        if(m_CLK < m_msAttack) {
            scaleFactor = (m_CLK / m_msAttack) * m_AttackLevel;
        }
        else if((m_CLK - m_msAttack) < m_msDecay) {
            scaleFactor = m_SustainLevel + ((1.0f - ((m_CLK - m_msAttack) / m_msDecay)) * (m_AttackLevel - m_SustainLevel));
        }
        else {
            scaleFactor = m_SustainLevel;
        }
        
        
        m_CLK += m_msCLKIncrement;
        m_releaseScaleFactor = scaleFactor;
    }
    else {
        if(m_releaseCLK > 0) {
            scaleFactor = (m_releaseCLK / m_msRelease) * m_releaseScaleFactor;
            
            m_releaseCLK -= m_msCLKIncrement;
        }
    }
    
    retVal *= scaleFactor;
    
    return retVal;
}

bool GtarSampleBuffer::IsNoteOn() {
    return m_fNoteOn;
}

void GtarSampleBuffer::NoteOn() {
    m_CLK = 0;
    m_releaseCLK = 0.0;
    m_msDecay = DEFAULT_MSDECAY;
    m_AttackLevel = DEFAULT_ATTACKLEVEL;
    m_msAttack = DEFAULT_MSATTACK;
    m_SustainLevel = DEFAULT_SUSTAINLEVEL;
    m_fNoteOn = true;
}

void GtarSampleBuffer::NoteMutedOn() {
    m_CLK = 0;
    m_releaseCLK = 0.0;
    m_msDecay = MUTED_MSDECAY;
    m_AttackLevel = MUTED_ATTACKLEVEL;
    m_msAttack = MUTED_MSATTACK;
    m_SustainLevel = MUTED_SUSTAINLEVEL;
    m_fNoteOn = true;
}

void GtarSampleBuffer::NoteSlideOn() {
    //m_CLK = 0;
    //m_msAttack = SLIDE_MSATTACK;
    //m_AttackLevel = SLIDE_ATTACKLEVEL;
    m_fNoteOn = true;
}

void GtarSampleBuffer::NoteOff() {
    
    if(m_fNoteOn && m_releaseCLK <= 0.0){
        m_releaseCLK = m_msRelease;
    }
    m_fNoteOn = false;
}

/*************************************/
// GtarSamplerNode
/*************************************/

GtarSamplerNode::GtarSamplerNode()
{
    SetChannelCount(1, CONN_OUT);
    m_fPlaying = FALSE;
    m_maxBank = -1;
    
    for(int i = 0; i < MAX_BANKS; i++){
        m_numSamples[i] = 0;
        m_nextSampleCounter[i] = 0;
    }
}

GtarSamplerNode::~GtarSamplerNode() {
    if(m_buffers != NULL) {
        for(int b = 0; b < MAX_BANKS; b++){
            for(int i = 0; i < MAX_SAMPLES; i++){
                m_buffers[b][i] = NULL;
            }
        }
    }
}

inline float GtarSamplerNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_fPlaying){
        for(int b = 0; b <= m_maxBank; b++){
            for(int i = 0; i <= m_numSamples[b]; i++){
                if(m_buffers[b][i] != NULL && m_buffers[b][i]->GtarSamplePlaying()){
                    
                    // turn off note on slide
                    int s = i;
                    if(m_sampleTransitionIndex[b][i] > -1 && m_sampleBufferTransitionIndex[b][i] == m_buffers[b][i]->m_pBuffer_c){
                        
                        s = m_sampleTransitionIndex[b][i];
                        m_buffers[b][s]->m_CLK = m_buffers[b][i]->m_CLK;
                        
                        m_buffers[b][s]->m_pBuffer_c = m_sampleBufferTransitionIndex[b][s]-1;
                        m_buffers[b][s]->NoteSlideOn();
                        
                        // turn off
                        m_buffers[b][i]->GtarSampleInterrupt();
                        
                        m_sampleBufferTransitionIndex[b][s] = -1;
                        m_sampleBufferTransitionIndex[b][i] = -1;
                        m_sampleTransitionIndex[b][i] = -1;
                        
                    }
                    retVal += m_buffers[b][s]->GtarGetNextSample(timestamp);
                    
                    if(m_buffers[b][s]->GtarSampleDone()) {
                        m_buffers[b][s]->ResetSampleCounter();
                    }
                }
            }
        }
    }
    
    return retVal;
}

RESULT GtarSamplerNode::ReleaseBank(int bank){
    RESULT r = R_SUCCESS;
    
    if(m_buffers != NULL) {
        for(int i = 0; i < MAX_SAMPLES; i++){
            if(m_buffers[bank][i] != NULL && m_buffers[bank][i]->m_pBuffer != NULL){
                free(m_buffers[bank][i]->m_pBuffer);
            }
            m_buffers[bank][i] = NULL;
        }
        m_nextSampleCounter[bank] = 0;
    }
    
Error:
    return r;
}

int GtarSamplerNode::CreateNewBank(int bank, int numSamples){
    
    if(bank >= MAX_BANKS){
        bank = 0;
    }
    
    // Save the max to minimize searching later
    m_maxBank = (bank > m_maxBank) ? bank : m_maxBank;
    
    // Ensure a bank is released before being reused
    ReleaseBank(bank);
    
    m_numSamples[bank] = numSamples;
    
    for(int i = 0; i < MAX_SAMPLES; i++)
    {
        m_sampleTransitionIndex[bank][i] = -1;
        m_sampleBufferTransitionIndex[bank][i] = -1;
    }
    
    return bank;
}

RESULT GtarSamplerNode::LoadSampleIntoBank(int bank, char *pszFilepath) {
    RESULT r = R_SUCCESS;
    
    m_buffers[bank][m_nextSampleCounter[bank]] = new GtarSampleBuffer(pszFilepath);
    
    m_nextSampleCounter[bank]++;
    m_nextSampleCounter[bank] %= m_numSamples[bank];
    
    return r;
    
Error:
    return r;
}

RESULT GtarSamplerNode::TriggerSample(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    m_buffers[bank][sample]->ResetSampleCounter();
    m_buffers[bank][sample]->GtarStartPlaying();
    m_buffers[bank][sample]->NoteOn();
    m_fPlaying = TRUE;
    
    return r;
    
Error:
    return r;
    
}

RESULT GtarSamplerNode::TriggerMutedSample(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    m_buffers[bank][sample]->ResetSampleCounter();
    m_buffers[bank][sample]->GtarStartPlaying();
    m_buffers[bank][sample]->NoteMutedOn();
    m_fPlaying = TRUE;
    
    return r;
    
Error:
    return r;
}

unsigned long int GtarSamplerNode::TriggerContinuousSample(int bank, int sampleLead, int sampleTrail) {
    
    if(sampleLead == sampleTrail || sampleTrail < 0 || m_buffers[bank][sampleLead]->m_pBuffer_c == 0){
        // TriggerSample(bank, sampleLead);
        return -1;
    }
    
    float currentSample;
    float currentNextSample;
    float nextSample;
    float nextNextSample;
    
    unsigned long int currentIndex = m_buffers[bank][sampleLead]->m_pBuffer_c;
    unsigned long int nextIndex;
    
    // Figure out where the two samples cross
    while (1)
    {
        // Out of bounds
        if(currentIndex > m_buffers[bank][sampleTrail]->m_pBuffer_end){
            return -3;
        }
        
        currentIndex++;
        currentSample = m_buffers[bank][sampleLead]->m_pBuffer[currentIndex];
        nextSample = m_buffers[bank][sampleTrail]->m_pBuffer[currentIndex];
        if(currentSample < 0 && nextSample >= 0){
            break;
        }
    }
    
    nextIndex = (unsigned long int) currentIndex * 0.9;
    
    // Rewind nextIndex
    while (1)
    {
        // Out of bounds
        if(nextIndex > m_buffers[bank][sampleTrail]->m_pBuffer_end){
            return -3;
        }
        
        currentSample = m_buffers[bank][sampleTrail]->m_pBuffer[currentIndex];
        currentNextSample = m_buffers[bank][sampleTrail]->m_pBuffer[currentIndex+1];
        
        nextSample = m_buffers[bank][sampleTrail]->m_pBuffer[nextIndex+1];
        nextNextSample = m_buffers[bank][sampleTrail]->m_pBuffer[nextIndex+2];
        
        if(currentSample == nextSample && ((currentNextSample > currentSample && nextNextSample > nextSample) || (currentNextSample < currentSample && nextNextSample < nextSample))){
            break;
        }
        nextIndex++;
    }
    
    // index to transition is currentIndex
    m_sampleBufferTransitionIndex[bank][sampleLead] = currentIndex;
    m_sampleBufferTransitionIndex[bank][sampleTrail] = nextIndex;
    m_sampleTransitionIndex[bank][sampleLead] = sampleTrail;
    
    m_buffers[bank][sampleTrail]->ResetSampleCounter();
    
    m_fPlaying = TRUE;
    
    return currentIndex;
    
}

RESULT GtarSamplerNode::StopSample(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    if(m_buffers[bank][sample] != NULL){
        m_buffers[bank][sample]->NoteOff();
        m_buffers[bank][sample]->ResetSampleCounter();
    }
    
    return r;
    
Error:
    return r;
}

RESULT GtarSamplerNode::NoteOff(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    if(m_buffers[bank][sample] != NULL){
        m_buffers[bank][sample]->NoteOff();
    }
    
    return r;
    
Error:
    return r;
}

bool GtarSamplerNode::IsNoteOn(int bank, int sample) {
    
    return m_buffers[bank][sample]->IsNoteOn();
    
}

