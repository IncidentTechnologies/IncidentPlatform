#include "FuzzExpDistortion.h"

FuzzExpDistortion::FuzzExpDistortion(double gain, double wet, double SamplingFrequency):
    Effect("Fuzz", wet, SamplingFrequency),
    m_gain(gain),
    m_runningAvg(0.0)
{
    /* stub */
}

inline double FuzzExpDistortion::InputSample(double sample)
{
    // protect against divide by zero errors
    if (0 == sample)
        return sample;
    
    if(m_fPassThrough)
        return sample;
    
    double retVal = 0;
    
    // calculate the maxima of the signal as simply 2 * the average
    m_runningAvg = 99.0 * m_runningAvg/100.0 + fabs(sample)/100.0;
    double currentMax = 2.0 * m_runningAvg;
    
    if (0 == currentMax)
        return sample;
    
    double normalizedSample = m_gain*sample/currentMax;
    double signOfSample = normalizedSample/fabs(normalizedSample);
    
    retVal = -signOfSample * (1 - expf(-signOfSample * normalizedSample));
    
    // reverse normalization back to scale of currentMax
    return retVal * currentMax;
}

bool FuzzExpDistortion::SetGain(double gain)
{
    if (gain < 1.0)
        return false;

    m_gain = gain;
    return true;
}