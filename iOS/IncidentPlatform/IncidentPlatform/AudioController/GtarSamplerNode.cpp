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
// GtarSamplerNode
/*************************************/

GtarSamplerNode::GtarSamplerNode() :
m_ppSampleBuffers(NULL)
{
    SetChannelCount(1, CONN_OUT);
    m_fPlaying = FALSE;
    m_nextSampleCounter = 0;
    m_numSamples = 0;
}

GtarSamplerNode::~GtarSamplerNode() {
    if(m_ppSampleBuffers != NULL) {
        delete m_ppSampleBuffers;
        m_ppSampleBuffers = NULL;
    }
}

float GtarSamplerNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    if(m_fPlaying){
        for(int i = 0; i < m_numSamples; i++){
            if(m_ppSampleBuffers != NULL && m_ppSampleBuffers[i]->SamplePlaying()){
                retVal += m_ppSampleBuffers[i]->GetNextSample(timestamp);
                
                if(m_ppSampleBuffers[i]->SampleDone()) {
                    m_ppSampleBuffers[i]->ResetSampleCounter();
                }
            }
        }
    }
    
    return retVal;
}

RESULT GtarSamplerNode::ReleaseBank(){
    RESULT r = R_SUCCESS;
    
    if(m_ppSampleBuffers != NULL) {
        delete m_ppSampleBuffers;
        m_ppSampleBuffers = NULL;
        m_nextSampleCounter = 0;
    }
    
Error:
    return r;
}

RESULT GtarSamplerNode::CreateNewBank(int numSamples){
    RESULT r = R_SUCCESS;
    
    ReleaseBank();

    m_numSamples = numSamples;
    
    m_ppSampleBuffers = (SampleBuffer**)malloc(sizeof(SampleBuffer*)*numSamples);
    
Error:
    return r;
}

RESULT GtarSamplerNode::LoadSampleIntoBank(char *pszFilepath) {
    RESULT r = R_SUCCESS;
    
    m_ppSampleBuffers[m_nextSampleCounter] = (SampleBuffer*)malloc(sizeof(SampleBuffer));
    m_ppSampleBuffers[m_nextSampleCounter] = new SampleBuffer(pszFilepath);
    
    m_nextSampleCounter++;
    m_nextSampleCounter %= m_numSamples;
    
    return r;
    
Error:
    return r;
}

RESULT GtarSamplerNode::TriggerSample(int sample) {
    
    m_ppSampleBuffers[sample]->ResetSampleCounter();
    m_ppSampleBuffers[sample]->StartPlaying();
    m_fPlaying = TRUE;
    return R_SUCCESS;
    
}

RESULT GtarSamplerNode::StopSample(int sample) {
    
    m_ppSampleBuffers[sample]->ResetSampleCounter();
    
    return R_SUCCESS;
}


