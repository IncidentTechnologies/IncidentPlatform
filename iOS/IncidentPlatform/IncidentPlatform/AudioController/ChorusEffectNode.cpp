//
//  ChorusEffectNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "ChorusEffectNode.h"
#define _USE_MATH_DEFINES

ChorusEffectNode::ChorusEffectNode(double msDelayTime, double depth, double width, double LFOFreq, double wet, double SamplingFrequency) :
    //Effect("Chorus", wet, SamplingFrequency),
    EffectNode(),
    m_pDelayLine(NULL),
    m_pDelayLine_n(0),
    m_SamplingFrequency(SamplingFrequency)
{
    
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    m_pDelayTime = new Parameter(msDelayTime, 0, CHORUS_EFFECT_MAX_MS_DELAY, "Delay");
    m_pDepth = new Parameter(depth, 0.0, 1.0, "Depth");
    m_pWidth = new Parameter(width, 0.0, 1.0, "Width");
    m_pLFOFreq = new Parameter(LFOFreq, 0.0, 10, "LFO Frequency");
    
    m_pDelayLine_n = (int)(((double) CHORUS_EFFECT_MAX_MS_DELAY / 1000.0f) * m_SamplingFrequency);
    m_pDelayLine = new double[m_pDelayLine_n];
    memset(m_pDelayLine, 0, sizeof(double) * m_pDelayLine_n);
    
    m_pDelayLine_l = (int)((m_pDelayTime->getValue() / 1000.f) * m_SamplingFrequency);
    m_pDelayLine_c = 0;
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float ChorusEffectNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

bool ChorusEffectNode::SetMSDelayTime(double msDelayTime)
{
    if (!m_pDelayTime->setValue(msDelayTime))
    {
        return false;
    }
    m_pDelayLine_l = (int)((m_pDelayTime->getValue() / 1000.f) * m_SamplingFrequency);
    m_pDelayLine_c = 0;
    return true;
}

bool ChorusEffectNode::SetDepth(double depth)
{
    return m_pDepth->setValue(depth);
}

bool ChorusEffectNode::SetLFOFreq(double freq)
{
    return m_pLFOFreq->setValue(freq);
}

bool ChorusEffectNode::SetWidth(double width)
{
    return m_pWidth->setValue(width);
}

inline double ChorusEffectNode::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    // place the new sample into the circular buffer
    m_pDelayLine[m_pDelayLine_c] = sample;
    
    // Get the feedback sample
    static long thetaN = 0;
    double ratio = 1 + m_pWidth->getValue() * sin(2.0f * M_PI * m_pLFOFreq->getValue() * (double)(thetaN / m_SamplingFrequency));
    
    long ratioL = m_pDelayLine_l * ratio;
    
    long feedBackSampleIndex = m_pDelayLine_c - ratioL;
    if(feedBackSampleIndex < 0)
        feedBackSampleIndex = (m_pDelayLine_c - ratioL) + m_pDelayLine_n;
    float feedBackSample = m_pDelayLine[feedBackSampleIndex];
    
    m_pDelayLine_c++;
    if(m_pDelayLine_c >= m_pDelayLine_n)
        m_pDelayLine_c = 0;
    
    // increment theta
    thetaN += 1;
    if(thetaN == m_SamplingFrequency)
        thetaN = 0;
    
    retVal = (1 - m_pWet->getValue()) * sample + m_pWet->getValue() * (sample + m_pDepth->getValue() * feedBackSample);
    
    return retVal;
}

void ChorusEffectNode::Reset()
{
    EffectNode::Reset();
    ClearOutEffect();
    SetDepth(0.75);
    SetLFOFreq(3.0);
}

void ChorusEffectNode::ClearOutEffect()
{
    memset(m_pDelayLine, 0, sizeof(double) * m_pDelayLine_n);
}


Parameter* ChorusEffectNode::getPrimaryParam()
{
    return m_pDepth;
}

bool ChorusEffectNode::setPrimaryParam(float value)
{
    return SetDepth(value);
}

Parameter* ChorusEffectNode::getSecondaryParam()
{
    return m_pLFOFreq;
}

bool ChorusEffectNode::setSecondaryParam(float value)
{
    return SetLFOFreq(value);
}

ChorusEffectNode::~ChorusEffectNode()
{
    if(m_pDelayLine != NULL)
    {
        delete [] m_pDelayLine;
        m_pDelayLine = NULL;
    }
}




