#include "FirstOrderFilterNode.h"

FirstOrderFilterNode::FirstOrderFilterNode(double feedback, double wet) :
    //Effect("1st order filter", wet, SamplingFrequency),
    EffectNode(),
    m_previousSample(0),
    m_feedback(feedback)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
}

inline double FirstOrderFilterNode::InputSample(double sample) {
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    double output = sample + m_feedback * m_previousSample;
    m_previousSample = output;
    
    retVal = (1 - m_pWet->getValue()) * sample + m_pWet->getValue() * output;
    return retVal;
}

bool FirstOrderFilterNode::SetFeedback(double feedback) {
    m_feedback = feedback;
    return true;
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float FirstOrderFilterNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return (float)InputSample(inVal);
}
    
