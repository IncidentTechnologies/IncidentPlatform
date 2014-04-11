
#include "DelayNode.h"

DelayNode::DelayNode(double msDelayTime, double feedback, double wet) :
    EffectNode(),
    m_pDelayLine_n(0),
    m_pDelayLine(NULL)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    SetWet(wet);
    
    // Set up the params
    m_pDelayTime = new Parameter(msDelayTime, 0.0, DELAY_EFFECT_MAX_MS_DELAY, "Delay");
    m_pFeedback = new Parameter(feedback, 0.0, 0.999, "Feedback");
    
    m_pDelayLine_n = (int)(((double) DELAY_EFFECT_MAX_MS_DELAY / 1000.0f) * m_SampleRate);
    m_pDelayLine = new double[m_pDelayLine_n];
    memset(m_pDelayLine, 0, sizeof(double) * m_pDelayLine_n);
    
    m_pDelayLine_l = (int)((m_pDelayTime->getValue() / 1000.f) * m_SampleRate);
    m_pDelayLine_c = 0;
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float DelayNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}

float DelayNode::InputSample(double sample) {
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    long feedBackSampleIndex = m_pDelayLine_c - m_pDelayLine_l;
    if(feedBackSampleIndex < 0)
        feedBackSampleIndex = (m_pDelayLine_c - m_pDelayLine_l) + m_pDelayLine_n;
    float feedBackSample = m_pDelayLine[feedBackSampleIndex];
    
    float newVal = sample + m_pFeedback->getValue() * feedBackSample;
    retVal = (1.0f - m_pWet->getValue()) * sample + m_pWet->getValue() * newVal;
    
    // place the new sample into the circular buffer
    m_pDelayLine[m_pDelayLine_c] = newVal;
    
    m_pDelayLine_c++;
    if(m_pDelayLine_c >= m_pDelayLine_n)
        m_pDelayLine_c = 0;
    
    return retVal;
}

float DelayNode::GetFeedback() {
    return m_pFeedback->getValue();
}

bool DelayNode::SetFeedback(float feedbackValue) {
    return m_pFeedback->setValue(feedbackValue);
}

float DelayNode::GetDelayTime(){
    return m_pDelayTime->getValue();
}

bool DelayNode::SetDelayTime(float delayTime) {
    if(!m_pDelayTime->setValue(delayTime))
        return false;
    
    m_pDelayLine_l = (int)((m_pDelayTime->getValue() / 1000.f) * m_SampleRate);
    m_pDelayLine_c = 0;
    return true;
}

Parameter* DelayNode::getPrimaryParam()
{
    return m_pDelayTime;
}

bool DelayNode::setPrimaryParam(float value)
{
    return SetDelayTime(value);
}

Parameter* DelayNode::getSecondaryParam()
{
    return m_pFeedback;
}

bool DelayNode::setSecondaryParam(float value)
{
    return SetFeedback(value);
}

void DelayNode::Reset() {
    EffectNode::Reset();
    ClearOutEffect();
    
    //setPrimaryParam(25); //delay
    //SetFeedback(0.5);
}

void DelayNode::ClearOutEffect() {
    memset(m_pDelayLine, 0, sizeof(double) * m_pDelayLine_n);
    m_pDelayLine_c = 0;
}

DelayNode::~DelayNode() {
    if(m_pDelayLine != NULL) {
        delete [] m_pDelayLine;
        m_pDelayLine = NULL;
    }
    
    if(m_pDelayTime != NULL) {
        delete m_pDelayTime;
        m_pDelayTime = NULL;
    }
    
    if(m_pFeedback != NULL) {
        delete m_pFeedback;
        m_pFeedback = NULL;
    }
}


