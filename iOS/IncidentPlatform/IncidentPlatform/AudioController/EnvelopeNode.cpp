//
//  EnvelopeNode.m
//  AudioController
//
//  Created by Idan Beck on 2/12/14.
//
//

#define DEFAULT_MSATTACK 10.0f
#define DEFAULT_ATTACKLEVEL 1.0f
#define DEFAULT_MSDECAY 100.0f
#define DEFAULT_SUSTAINLEVEL 1.0f
#define DEFAULT_MSRELEASE 200.0f

#include "EnvelopeNode.h"

EnvelopeNode::EnvelopeNode() :
    AudioNode(),
    m_msAttack(DEFAULT_MSATTACK),
    m_AttackLevel(DEFAULT_ATTACKLEVEL),
    m_msDecay(DEFAULT_MSDECAY),
    m_SustainLevel(DEFAULT_SUSTAINLEVEL),
    m_msRelease(DEFAULT_MSRELEASE),
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
    float inVal = AudioNode::GetNextSample(timestamp); // first get inputs
    return InputSample(inVal);

}

inline double EnvelopeNode::InputSample(double sample)
{
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
    
    sample *= scaleFactor;
    
    return sample;
}

EnvelopeNode::~EnvelopeNode()
{
    
}