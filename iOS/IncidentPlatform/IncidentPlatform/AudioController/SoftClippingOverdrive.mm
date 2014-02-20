#include "SoftClippingOverdrive.h"


SoftClippingOverdrive::SoftClippingOverdrive (double threshold,double multiplier, double wet, double SamplingFrequency) :
    Effect("Softclip distortion", wet, SamplingFrequency),
    m_threshold(threshold),
    m_multiplier(multiplier)
{
    m_pLocalMax = new LocalMax(m_SamplingFrequency);
}
    
inline double SoftClippingOverdrive::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    double currentMax = m_pLocalMax->GetLocalMax(sample);
    double thresHold = m_threshold;
    double normalizedSample = sample/currentMax;
    double absSample = fabs(normalizedSample);
    if(absSample<thresHold)
    {
        retVal=(sample*2*m_multiplier);
    }
    else if(absSample<2*thresHold)
    {
        if(sample>0)
            retVal= ((3-(2-sample*3)*(2-sample*3))/3);
        else if(sample<0)
            retVal= (-(3-(2-absSample*3)*(2-absSample*3))/3);
    }
    else if(absSample>=2*thresHold)
    {
        if(sample>0)
            retVal=1;
        else if(sample<0)
            retVal=-1;
    }
    
    // reverse normalization back to scale of currentMax
    retVal = retVal * currentMax;
    return retVal;
}
    
bool SoftClippingOverdrive::SetThreshold(double threshold)
{
    if(threshold > 1.0f || threshold < 0.0) return false;
    m_threshold = threshold;
    return true;
}

bool SoftClippingOverdrive::SetMultiplier(double multiplier)
{
    m_multiplier = multiplier;
    return true;
}

SoftClippingOverdrive::~SoftClippingOverdrive()
{
    delete m_pLocalMax;
    m_pLocalMax = NULL;
}
