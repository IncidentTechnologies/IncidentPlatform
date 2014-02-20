//
//  EnvelopeNode.m
//  AudioController
//
//  Created by Idan Beck on 2/12/14.
//
//

#include "EnvelopeNode.h"

EnvelopeNode::EnvelopeNode() :
    AudioNode(),
    m_msAttack(100.0f),
    m_AttackLevel(1.0f),
    m_msDecay(200.0f),
    m_SustainLevel(0.25f),
    m_msRelease(1000.0f),
    m_CLK(0.0f),
    m_releaseCLK(0.0f)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    m_fNoteOn = false;
    m_msCLKIncrement = 1000.0f / m_SampleRate;
}

bool EnvelopeNode::IsNoteOn() {
    return m_fNoteOn;
}

void EnvelopeNode::NoteOn() {
    m_CLK = 0;
    m_fNoteOn = true;
}

void EnvelopeNode::NoteOff() {
    m_fNoteOn = false;
    m_releaseCLK = m_msRelease;
}

float EnvelopeNode::GetNextSample(unsigned long int timestamp) {
    float retVal = AudioNode::GetNextSample(timestamp); // first get inputs
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