#include "HardCutOffDistortion.h"

HardCutoffDistortion::HardCutoffDistortion (double cutoff, double wet, double SamplingFrequency) :
    Effect("Hard cutoff distortion", wet, SamplingFrequency),
m_cutoff(cutoff)
{
    m_pLocalMax = new LocalMax(m_SamplingFrequency);
}
    
bool HardCutoffDistortion::SetCutoff(double cutoff)
{
    if(cutoff > 1.0f || cutoff < 0.0) return false;
    m_cutoff = cutoff;
    return true;
}

inline double HardCutoffDistortion::InputSample(double sample)
{
    if(m_fPassThrough)
        return sample;
    
    double retVal;
    
    // cutoff value will be a percentage (m_cutoff) of the local max
    double currentMax = m_pLocalMax->GetLocalMax(sample);
    double scaledCutoff = currentMax * m_cutoff;
    
    if (sample > scaledCutoff)
    {
        retVal = scaledCutoff;
    }
    else if (sample < -scaledCutoff)
    {
        retVal =  -scaledCutoff;
    }
    else
    {
        retVal = sample;
    }
    
    // scale output so that cutoff is scaled up to inputs max amplitude (local max)
    retVal = retVal/m_cutoff;
    return retVal;
}

HardCutoffDistortion::~HardCutoffDistortion()
{
    delete m_pLocalMax;
    m_pLocalMax = NULL;
}
    


