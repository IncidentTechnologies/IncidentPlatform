#include "Overdrive.h"

Overdrive::Overdrive(double gain, double wet, double SamplingFrequency) :
    Effect("Overdrive", wet, SamplingFrequency),
    m_gain(gain)
{
    if(m_gain < 1.0) m_gain = 1.0f;
}
    
bool Overdrive::SetGain(double gain)
{
    if(gain < 1.0f) return false;
    m_gain = gain;
    return true;
}

inline double Overdrive::InputSample(double sample)
{
    if(m_fPassThrough)
        return sample;
    
    double retVal = 0;
    retVal = (1.0f - m_pWet->getValue()) * (sample) + (m_pWet->getValue()) * (sample * m_gain);
    return retVal;
}

Overdrive::~Overdrive()
{
    /* empty stub */
}