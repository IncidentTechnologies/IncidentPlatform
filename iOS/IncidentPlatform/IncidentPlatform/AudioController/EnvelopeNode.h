//
//  EnvelopeNode.h
//  AudioController
//
//  Created by Idan Beck on 2/12/14.
//
//

#ifndef IncidentPlatform_EnvelopeNode_h
#define IncidentPlatform_EnvelopeNode_h

#import "AudioNode.h"

class EnvelopeNode : public AudioNode {
public:
    EnvelopeNode();
    
    void NoteOn();
    void NoteOff();
    bool IsNoteOn();
    
    inline double InputSample(double sample);
    float GetNextSample(unsigned long int timestamp);
    
    ~EnvelopeNode();
    
private:
    int m_channel_n;
    float m_CLK;
    float m_msCLKIncrement;
    float m_releaseCLK;
    bool m_fNoteOn;
    
public:
    float m_msAttack;
    float m_AttackLevel;
    
    float m_msDecay;
    float m_SustainLevel;
    
    float m_msRelease;
    float m_ReleaseLevel;
    
    float m_releaseScaleFactor;
};

#endif