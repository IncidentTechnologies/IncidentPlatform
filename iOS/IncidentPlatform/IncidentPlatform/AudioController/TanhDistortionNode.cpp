//
//  TanhDistortionNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "TanhDistortionNode.h"

TanhDistortionNode::TanhDistortionNode (double gain, double wet, double SamplingFrequency) :
//Effect("Distortion", wet, SamplingFrequency)
EffectNode()
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    m_pPosFactor = new Parameter(1.0, 1.0, 100, "Positive Dist");
    m_pNegFactor = new Parameter(1.0, 1.0, 100, "Negative Dist");
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float TanhDistortionNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

double TanhDistortionNode::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    float factor;
    if (sample > 0)
    {
        factor = m_pPosFactor->getValue();
        retVal = tanh(factor*sample)/factor;
    }
    else
    {
        factor = m_pNegFactor->getValue();
        retVal = tanh(factor*sample)/factor;
    }
    
    return retVal * 2;
}

bool TanhDistortionNode::setPosFactor(double factor)
{
    return m_pPosFactor->setValue(factor);
}

bool TanhDistortionNode::setNegFactor(double factor)
{
    return m_pNegFactor->setValue(factor);
}

Parameter* TanhDistortionNode::getPrimaryParam()
{
    return m_pPosFactor;
}

bool TanhDistortionNode::setPrimaryParam(float value)
{
    return m_pPosFactor->setValue(value);
}

Parameter* TanhDistortionNode::getSecondaryParam()
{
    return m_pNegFactor;
}

bool TanhDistortionNode::setSecondaryParam(float value)
{
    return m_pNegFactor->setValue(value);
}

TanhDistortionNode::~TanhDistortionNode()
{
    delete m_pPosFactor;
    m_pPosFactor = NULL;
    
    delete m_pNegFactor;
    m_pNegFactor = NULL;
}



