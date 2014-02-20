//
//  WavetableOscillatorNode.m
//  AudioController
//
//  Created by Idan Beck on 2/12/14.
//
//

#include "WavetableNode.h"

fnWavetable WavetableFunctions[] = {
    WavetableSine,
    WavetableSaw,
    WavetableSquare,
    WavetableTriangle,
    WavetableInvalid
};

float WavetableSine(float theta) {
    return sin(2.0f * M_PI * theta);
}

float WavetableSaw(float theta) {
    return (theta - 0.5f) * 2.0f;
}

float WavetableSquare(float theta) {
    if(theta < 0.5f)
        return 1.0f;
    else
        return -1.0f;
}

float WavetableTriangle(float theta) {
    if(theta < 0.5f)
        return ((theta * 2.0f) - 0.5f) * 2.0f;
    else
        return (((1.0f - theta) * 2.0f) - 0.5f) * 2.0f;
}

float WavetableInvalid(float theta){
    return 0.0f;
}

WavetableNode::WavetableNode() :
    GeneratorNode(),
    m_frequency(440.0f),
    m_phase(0.0f),
    m_theta(0.0f),
    m_type(WAVETABLE_SAW)
{
    SetChannelCount(1, CONN_OUT);
    
    InitializeWavetable();
}

void WavetableNode::InitializeWavetable() {
    float wavetableIncrement = 1.0f / WAVETABLE_LENGTH;
    
    for(int i = 0; i < WAVETABLE_LENGTH; i++)
        m_wavetable[i] = WavetableFunctions[m_type](i * wavetableIncrement);
}

void WavetableNode::trigger() {
    m_theta = 0.0f;
}

float WavetableNode::SetFrequency(float freq) { return (m_frequency = freq); }

float WavetableNode::SetPhaseOffset(float phase){
    return (m_phase = (phase / (2.0f * M_PI)));
}

float WavetableNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;

    float LUValue = ((m_theta + m_phase) * m_frequency) * WAVETABLE_LENGTH;

    if(LUValue >= WAVETABLE_LENGTH)
        LUValue = (float)(((int)floorf(LUValue) % (WAVETABLE_LENGTH)) + (float)(LUValue - floorf(LUValue)));

    // interpolate between samples
    int LUfloor = floorf(LUValue);
    int LUceil = ceilf(LUValue);
    float ratio = LUValue - LUfloor;
    
    retVal = m_wavetable[LUfloor] * (1.0f - ratio) + m_wavetable[LUceil] * ratio;

    
    m_theta += (1.0f / m_SampleRate);
    if(m_theta >= 1.0f)
        m_theta -= 1.0f;
    
    return retVal;
}
