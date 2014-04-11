//
//  SoftClippingOverdriveNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "SoftClippingOverdriveNode.h"


SoftClippingOverdriveNode::SoftClippingOverdriveNode (double threshold,double multiplier, double wet, double SamplingFrequency) :
//Effect("Softclip distortion", wet, SamplingFrequency),
EffectNode(),
m_threshold(threshold),
m_multiplier(multiplier)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    //m_pLocalMax = new LocalMax(m_SamplingFrequency);
    m_pLocalMax = new LocalMax(SamplingFrequency);
}

inline double SoftClippingOverdriveNode::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    double currentMax = m_pLocalMax->GetLocalMax(sample);
    double thresHold = m_threshold;
    double normalizedSample = sample/currentMax;
    double absSample = fabs(normalizedSample);
    if(absSample<thresHold)
    {
        retVal=(sample*2*m_multiplier);
    }
    else if(absSample<2*thresHold)
    {
        if(sample>0)
            retVal= ((3-(2-sample*3)*(2-sample*3))/3);
        else if(sample<0)
            retVal= (-(3-(2-absSample*3)*(2-absSample*3))/3);
    }
    else if(absSample>=2*thresHold)
    {
        if(sample>0)
            retVal=1;
        else if(sample<0)
            retVal=-1;
    }
    
    // reverse normalization back to scale of currentMax
    retVal = retVal * currentMax;
    return retVal;
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float SoftClippingOverdriveNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

bool SoftClippingOverdriveNode::SetThreshold(double threshold)
{
    if(threshold > 1.0f || threshold < 0.0) return false;
    m_threshold = threshold;
    return true;
}

bool SoftClippingOverdriveNode::SetMultiplier(double multiplier)
{
    m_multiplier = multiplier;
    return true;
}

SoftClippingOverdriveNode::~SoftClippingOverdriveNode()
{
    delete m_pLocalMax;
    m_pLocalMax = NULL;
}
