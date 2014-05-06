//
//  OverdriveNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "OverdriveNode.h"

OverdriveNode::OverdriveNode(double gain, double wet, double SamplingFrequency) :
//Effect("Overdrive", wet, SamplingFrequency),
EffectNode(),
m_gain(gain)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    if(m_gain < 1.0) m_gain = 1.0f;
}

bool OverdriveNode::SetGain(double gain)
{
    if(gain < 1.0f) return false;
    m_gain = gain;
    return true;
}

inline double OverdriveNode::InputSample(double sample)
{
    if(m_fPassThrough)
        return sample;
    
    double retVal = 0;
    retVal = (1.0f - m_pWet->getValue()) * (sample) + (m_pWet->getValue()) * (sample * m_gain);
    return retVal;
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float OverdriveNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

OverdriveNode::~OverdriveNode()
{
    /* empty stub */
}