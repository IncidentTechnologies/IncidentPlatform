//
//  DistortionNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "DistortionNode.h"

DistortionNode::DistortionNode (double gain, double wet, double SamplingFrequency) :
//Effect("Distortion", wet, SamplingFrequency)
EffectNode()
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    m_pDrive = new Parameter(defaultDrive, 1.0f, 20.0f, "Drive");
    m_pFuzzGain = new Parameter(defaultFuzz, 1.0f, 20.0f, "Fuzz");
    
    m_gain = gain;
    m_pTanhDistortion = new TanhDistortionNode(1.0, 1.0, SamplingFrequency);
    setPrimaryParam(m_pDrive->getValue());
    m_pFuzzExpDistortion = new FuzzExpDistortionNode(m_pFuzzGain->getValue(), 1.0, SamplingFrequency);
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float DistortionNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

inline double DistortionNode::InputSample(double sample)
{
    if (m_fPassThrough)
        return sample;
    
    float tempSample = 0.0f;
    tempSample = m_pTanhDistortion->InputSample(sample);
    tempSample = tempSample * m_gain;
    tempSample = m_pFuzzExpDistortion->InputSample(tempSample);
    
    tempSample = (1.0f - m_pWet->getValue()) * (sample) + (m_pWet->getValue()) * (tempSample);
    
    return tempSample;
}

Parameter* DistortionNode::getPrimaryParam()
{
    return m_pDrive;
}

// This param is the negDistortion param of the TanhDistortion unit. Here we
// will also set the posDistortion param based on the neg value.
bool DistortionNode::setPrimaryParam(float value)
{
    m_pDrive->setValue(value);
    // from testing we have come up with this formula for relating the
    // posDist param from a given negDist param for the tanh dist
    // posDist = 3.66*negDist - 2.66
    m_pTanhDistortion->setPrimaryParam(3.66f * value - 2.66f);
    return m_pTanhDistortion->setSecondaryParam(value);
}

Parameter* DistortionNode::getSecondaryParam()
{
    return m_pFuzzGain;
}

bool DistortionNode::setSecondaryParam(float value)
{
    m_pFuzzGain->setValue(value);
    return m_pFuzzExpDistortion->SetGain(value);
}

void DistortionNode::Reset()
{
    EffectNode::Reset();
    setPrimaryParam(defaultDrive);
    setSecondaryParam(defaultFuzz);
}

DistortionNode::~DistortionNode()
{
    delete m_pDrive;
    m_pDrive = NULL;
    
    delete m_pFuzzGain;
    m_pFuzzGain = NULL;
    
    delete m_pTanhDistortion;
    m_pTanhDistortion = NULL;
    
    delete m_pFuzzExpDistortion;
    m_pFuzzExpDistortion = NULL;
}

