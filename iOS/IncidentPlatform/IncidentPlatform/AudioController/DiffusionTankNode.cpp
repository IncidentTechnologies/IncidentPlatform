
#include "DiffusionTankNode.h"

DiffusionTankNode::DiffusionTankNode(double msDelayTime, double feedback, bool posOutputSum, double wet):
    DelayNode(msDelayTime, feedback, wet)
{
    if (posOutputSum)
        m_OutputSumSign = 1.0;
    else
        m_OutputSumSign = -1.0;
}

double DiffusionTankNode::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    m_delayedSampleIndex = m_pDelayLine_c - m_pDelayLine_l;
    
    if(m_delayedSampleIndex < 0)
        m_delayedSampleIndex = m_delayedSampleIndex + m_pDelayLine_n;
    
    float delayedSample = m_pDelayLine[m_delayedSampleIndex];
    
    // feedback and feedforward
    double intoDelayLine = sample - m_OutputSumSign * delayedSample * m_pFeedback->getValue();
    retVal = delayedSample + m_OutputSumSign * intoDelayLine * m_pFeedback->getValue();
    
    // place the new sample into the circular buffer
    m_pDelayLine[m_pDelayLine_c] = intoDelayLine;
    
    m_pDelayLine_c++;
    if(m_pDelayLine_c >= m_pDelayLine_n)
        m_pDelayLine_c = 0;
    
    return retVal;
}

double DiffusionTankNode::GetSample(long offset)
{
    long index = m_delayedSampleIndex + offset;
    if (index >= m_pDelayLine_n)
        index -= m_pDelayLine_n;
    
    return m_pDelayLine[index];
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float DiffusionTankNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}
