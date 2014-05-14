//
//  GtarSamplerNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 5/13/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "GtarSamplerNode.h"
#include "SampleNode.h"
#include "CAXException.h"
#include <AudioToolbox/AudioFormat.h>

/*************************************/
// GtarSampleBuffer
/*************************************/

// Envelope control for Gtar
GtarSampleBuffer::GtarSampleBuffer(char *pszFilenamePath) :
    SampleBuffer(pszFilenamePath),
    m_msAttack(10.0f),
    m_AttackLevel(1.0f),
    m_msDecay(100.0f),
    m_SustainLevel(1.0f),
    m_msRelease(1000.0f),
    m_CLK(0.0f),
    m_releaseCLK(0.0f)
{
    LoadSampleBufferFromPath(pszFilenamePath);
    m_fNoteOn = false;
    m_msCLKIncrement = 1000.0f / DEFAULT_SAMPLE_RATE;
}

GtarSampleBuffer::~GtarSampleBuffer() {
    if(m_pBuffer != NULL) {
        free(m_pBuffer);
        m_pBuffer = NULL;
    }
}

bool GtarSampleBuffer::GtarSampleDone() {
    return (m_pBuffer_c >= m_pBuffer_end);
}

RESULT GtarSampleBuffer::GtarResetSampleCounter() {
    m_pBuffer_c = m_pBuffer_start;
    return R_SUCCESS;
}

RESULT GtarSampleBuffer::GtarStartPlaying() {
    
    m_pBuffer_c++;
    
    return R_SUCCESS;
}

bool GtarSampleBuffer::GtarSamplePlaying() {
    
    return (m_pBuffer_c > m_pBuffer_start && m_pBuffer_c < m_pBuffer_end);
}

float GtarSampleBuffer::GtarGetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_pBuffer != NULL && m_pBuffer_n > 0 && m_pBuffer_end > 0 && m_pBuffer_c < m_pBuffer_end) {
        retVal = m_pBuffer[m_pBuffer_c];
        m_pBuffer_c++;
    }
    
    return EnvelopeSample(retVal);
}

float GtarSampleBuffer::EnvelopeSample(float retVal){
    
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
    m_fNoteOn = true;
}

void GtarSampleBuffer::NoteOff() {
    m_fNoteOn = false;
    m_releaseCLK = m_msRelease;
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

float GtarSamplerNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_fPlaying){
        for(int b = 0; b <= m_maxBank; b++){
            for(int i = 0; i <= m_numSamples[b]; i++){
                if(m_buffers[b][i] != NULL && m_buffers[b][i]->GtarSamplePlaying()){
                    
                    retVal += m_buffers[b][i]->GtarGetNextSample(timestamp);
                    
                    if(m_buffers[b][i]->GtarSampleDone()) {
                        m_buffers[b][i]->GtarResetSampleCounter();
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
    
    m_buffers[bank][sample]->GtarResetSampleCounter();
    m_buffers[bank][sample]->GtarStartPlaying();
    m_buffers[bank][sample]->NoteOn();
    m_fPlaying = TRUE;
    
    return r;
    
Error:
    return r;
    
}

RESULT GtarSamplerNode::StopSample(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    if(m_buffers[bank][sample] != NULL){
        m_buffers[bank][sample]->NoteOff();
        m_buffers[bank][sample]->GtarResetSampleCounter();
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


