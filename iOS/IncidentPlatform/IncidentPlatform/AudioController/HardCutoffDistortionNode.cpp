//
//  HardCutoffDistortionNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "HardCutOffDistortionNode.h"

HardCutoffDistortionNode::HardCutoffDistortionNode (double cutoff, double wet, double SamplingFrequency) :
//Effect("Hard cutoff distortion", wet, SamplingFrequency),
EffectNode(),
m_cutoff(cutoff)
{
    
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    //m_pLocalMax = new LocalMax(m_SamplingFrequency);
    m_pLocalMax = new LocalMax(SamplingFrequency);
}

bool HardCutoffDistortionNode::SetCutoff(double cutoff)
{
    if(cutoff > 1.0f || cutoff < 0.0) return false;
    m_cutoff = cutoff;
    return true;
}

inline double HardCutoffDistortionNode::InputSample(double sample)
{
    if(m_fPassThrough)
        return sample;
    
    double retVal;
    
    // cutoff value will be a percentage (m_cutoff) of the local max
    double currentMax = m_pLocalMax->GetLocalMax(sample);
    double scaledCutoff = currentMax * m_cutoff;
    
    if (sample > scaledCutoff)
    {
        retVal = scaledCutoff;
    }
    else if (sample < -scaledCutoff)
    {
        retVal =  -scaledCutoff;
    }
    else
    {
        retVal = sample;
    }
    
    // scale output so that cutoff is scaled up to inputs max amplitude (local max)
    retVal = retVal/m_cutoff;
    return retVal;
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float HardCutoffDistortionNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

HardCutoffDistortionNode::~HardCutoffDistortionNode()
{
    delete m_pLocalMax;
    m_pLocalMax = NULL;
}



