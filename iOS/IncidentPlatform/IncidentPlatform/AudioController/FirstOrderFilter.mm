#include "FirstOrderFilter.h"

FirstOrderFilter::FirstOrderFilter(double feedback, double wet, double SamplingFrequency) :
    Effect("1st order filter", wet, SamplingFrequency),
m_previousSample(0),
m_feedback(feedback)
{
    
}

inline double FirstOrderFilter::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    double output = sample + m_feedback * m_previousSample;
    m_previousSample = output;
    
    retVal = (1 - m_pWet->getValue()) * sample + m_pWet->getValue() * output;
    return retVal;
}

bool FirstOrderFilter::SetFeedback(double feedback)
{
    m_feedback = feedback;
    return true;
}
    
