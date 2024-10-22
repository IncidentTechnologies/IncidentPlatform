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
#define DEFAULT_MSRELEASE 200.0f
#define DEFAULT_NORMALSCALE 1.0f

#define DEFAULT_NORMAL_MAX 0.3f
#define DEFAULT_NORMAL_MIN -0.3f

#define MUTED_MSATTACK 10.0f
#define MUTED_ATTACKLEVEL 0.3f
#define MUTED_SUSTAINLEVEL 0.0f
#define MUTED_MSDECAY 200.0f

#define INSTANT_RELEASE 1.0f

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
    m_normalScale = DEFAULT_NORMALSCALE;
    
    NormalizeSample();
}

GtarSampleBuffer::GtarSampleBuffer(void *buffer, unsigned long int bufferLength) :
SampleBuffer(buffer, bufferLength),
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
    m_normalScale = DEFAULT_NORMALSCALE;
    
    NormalizeSample();
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
    
    retVal *= m_normalScale;
    
    return retVal;
}

inline RESULT GtarSampleBuffer::NormalizeSample() {
    RESULT r = R_SUCCESS;
    
    if(m_pBuffer != NULL && m_pBuffer_n > 0){

        double buffer_max = m_pBuffer[m_pBuffer_start];
        double buffer_min = m_pBuffer[m_pBuffer_start];
        
        for(unsigned long int c = m_pBuffer_start; c < m_pBuffer_end; c++){
            buffer_max = (m_pBuffer[c] > buffer_max) ? m_pBuffer[c] : buffer_max;
            buffer_min = (m_pBuffer[c] < buffer_min || buffer_min == 0) ? m_pBuffer[c] : buffer_min;
        }
        
        if(buffer_max > DEFAULT_NORMAL_MAX){
            
            m_normalScale = DEFAULT_NORMAL_MAX / buffer_max;
            
        }else if(buffer_min < DEFAULT_NORMAL_MIN){
            
            m_normalScale = fabsf(DEFAULT_NORMAL_MIN / buffer_min);
            
        }else if(buffer_max > fabsf(buffer_min)){
            
            m_normalScale = DEFAULT_NORMAL_MAX / buffer_max;
            
        }else if(fabsf(buffer_min) > buffer_max){
            
            m_normalScale = fabsf(DEFAULT_NORMAL_MIN / buffer_min);
            
        }
    }
    
    return r;
Error:
    return r;
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

void GtarSampleBuffer::StopNote() {
    
    if(m_fNoteOn){
        m_msAttack = 0.0;
        m_AttackLevel = 0.0;
        m_SustainLevel = 0.0;
        m_releaseCLK = INSTANT_RELEASE;
        m_CLK = m_releaseCLK;
        m_msDecay = 0.0;
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
    
    m_pRewind = new Parameter(0.95, 0.6, 1.0, "Rewind");
    m_pTransition = new Parameter(5, 3, 10, "Transition");
    
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
                        
                        m_buffers[b][s]->m_pBuffer_c = m_sampleBufferTransitionIndex[b][s];
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

RESULT GtarSamplerNode::LoadSampleIntoBankAtIndex(int bank, int index, char *pszFilepath) {
    RESULT r = R_SUCCESS;
    
    m_buffers[bank][index] = new GtarSampleBuffer(pszFilepath);
        
    return r;
    
Error:
    return r;
}


RESULT GtarSamplerNode::LoadSampleStringIntoBank(int bank, const void *sampleBuffer, unsigned long int bufferLength) {
    RESULT r = R_SUCCESS;
    
    m_buffers[bank][m_nextSampleCounter[bank]] = new GtarSampleBuffer((void *)sampleBuffer,bufferLength);
    
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

RESULT GtarSamplerNode::RetriggerSample(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    // Find a start that matches where the note leaves off
    
    unsigned long int currentSampleIndex = m_buffers[bank][sample]->m_pBuffer_c;
    unsigned long int prevSampleIndex = (currentSampleIndex > m_buffers[bank][sample]->m_pBuffer_start) ? currentSampleIndex-1 : m_buffers[bank][sample]->m_pBuffer_start;
    unsigned long int nextSampleIndex = (currentSampleIndex < m_buffers[bank][sample]->m_pBuffer_end-1) ? currentSampleIndex+1 : m_buffers[bank][sample]->m_pBuffer_end;
    
    float prevSample = m_buffers[bank][sample]->m_pBuffer[prevSampleIndex];
    float currentSample = m_buffers[bank][sample]->m_pBuffer[currentSampleIndex];
    float nextSample = m_buffers[bank][sample]->m_pBuffer[nextSampleIndex];
    
    // TODO: consider other logic cases
    bool isUp = (prevSample < currentSample && nextSample > currentSample);
    
    unsigned long int newSampleIndex = m_buffers[bank][sample]->m_pBuffer_start;
    
    unsigned long int sampleDiff = 0;
    
    // Search the first part of the sample to try to find a match
    
    bool foundNewSampleIndex = false;
    
    for(unsigned long int k = 1; k < 3; k++){
        for(unsigned long int i = m_buffers[bank][sample]->m_pBuffer_start+k; i < (m_buffers[bank][sample]->m_pBuffer_start + 0.5 * m_buffers[bank][sample]->m_pBuffer_n)-k; i++){
            
            if(isUp && m_buffers[bank][sample]->m_pBuffer[i-k] < currentSample && m_buffers[bank][sample]->m_pBuffer[i+k] > currentSample){
                
                foundNewSampleIndex = true;
                newSampleIndex = i-1;
                sampleDiff = k;
                break;
           
            }else if(!isUp && m_buffers[bank][sample]->m_pBuffer[i-k] > currentSample && m_buffers[bank][sample]->m_pBuffer[i+k] < currentSample){
                
                foundNewSampleIndex = true;
                newSampleIndex = i-1;
                sampleDiff = k;
                break;
                
            }
        }
        
        if(foundNewSampleIndex){
            break;
        }
    }
    
    if(!foundNewSampleIndex){
        printf("retrigger sample not found");
    }
    
    //printf("\nretrigger sample val %f at index %lu=%f | k is %lu | isUp is %i\n",currentSample,newSampleIndex,m_buffers[bank][sample]->m_pBuffer[newSampleIndex],sampleDiff,isUp);
    
    m_buffers[bank][sample]->m_pBuffer_c = newSampleIndex;
    
    m_buffers[bank][sample]->GtarStartPlaying();
    //m_buffers[bank][sample]->NoteOn();
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
    
    if(sampleLead == sampleTrail || sampleTrail < 0){
        return -1;
    }
    
    float currentPrevSample;
    float currentNextSample;
    float nextPrevSample;
    float nextNextSample;
    
    float nextPercent = m_pRewind->getValue();
    //float nextPercent = 0.95;
    int transitionOffset = 5; // m_pTransition->getValue();
    
    unsigned long int currentIndex = m_buffers[bank][sampleLead]->m_pBuffer_c+1;
    
    float currentPercent = (float)currentIndex / (float)(m_buffers[bank][sampleLead]->m_pBuffer_start+m_buffers[bank][sampleLead]->m_pBuffer_n);
    
    unsigned long int initialNextIndex = m_buffers[bank][sampleTrail]->m_pBuffer_start + currentPercent * nextPercent * m_buffers[bank][sampleTrail]->m_pBuffer_n;
    
    unsigned long int nextIndex = initialNextIndex;
    
    // Fast forward Current to Zero
    for(; currentIndex < m_buffers[bank][sampleLead]->m_pBuffer_end-transitionOffset; currentIndex++){
        
        currentPrevSample = m_buffers[bank][sampleLead]->m_pBuffer[currentIndex - 1] * m_buffers[bank][sampleLead]->m_normalScale;
        currentNextSample = m_buffers[bank][sampleLead]->m_pBuffer[currentIndex + 1] * m_buffers[bank][sampleLead]->m_normalScale;
        
        if(currentPrevSample < 0 && currentNextSample >= 0){
            break;
        }
        
    }
    
    // Rewind Next to Zero
    for(; nextIndex >= m_buffers[bank][sampleTrail]->m_pBuffer_start+transitionOffset; nextIndex--){
        
        nextPrevSample = m_buffers[bank][sampleTrail]->m_pBuffer[nextIndex] * m_buffers[bank][sampleTrail]->m_normalScale;
        nextNextSample = m_buffers[bank][sampleTrail]->m_pBuffer[nextIndex+1] * m_buffers[bank][sampleTrail]->m_normalScale;
        
        if(nextPrevSample < 0 && nextNextSample >= 0){
            break;
        }
        
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
        
        //m_buffers[bank][sample]->GtarSampleInterrupt();
        
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
        
        //m_buffers[bank][sample]->GtarSampleInterrupt();
        
        m_buffers[bank][sample]->NoteOff();
    }
    
    return r;
    
Error:
    return r;
}

RESULT GtarSamplerNode::StopNote(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    if(m_buffers[bank][sample] != NULL){
        
        //m_buffers[bank][sample]->GtarSampleInterrupt();
        
        m_buffers[bank][sample]->StopNote();
    }
    
    return r;
    
Error:
    return r;
}

bool GtarSamplerNode::IsNoteOn(int bank, int sample) {
    
    if(m_buffers[bank][sample] != NULL){
        return m_buffers[bank][sample]->IsNoteOn();
    }else{
        return true;
    }
}

bool GtarSamplerNode::IsDoubleTrigger(int bank, int sample){
    
    double threshold = 0.02;
    
    if(m_buffers[bank][sample]->m_pBuffer_c > m_buffers[bank][sample]->m_pBuffer_start && m_buffers[bank][sample]->m_pBuffer_c < threshold*m_buffers[bank][sample]->m_pBuffer_n+m_buffers[bank][sample]->m_pBuffer_start){
        return true;
    }else{
        return false;
    }
    
}

Parameter *GtarSamplerNode::getPrimaryParam()
{
    return m_pRewind;
}


bool GtarSamplerNode::setPrimaryParam(float value)
{
    return m_pRewind->setValue(value);
}

Parameter *GtarSamplerNode::getSecondaryParam()
{
    return m_pTransition;
}


bool GtarSamplerNode::setSecondaryParam(float value)
{
    return m_pTransition->setValue(value);
}

