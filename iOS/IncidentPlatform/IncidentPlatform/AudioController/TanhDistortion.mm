#include "TanhDistortion.h"

TanhDistortion::TanhDistortion (double gain, double wet, double SamplingFrequency) :
    Effect("Distortion", wet, SamplingFrequency)
{
    m_pPosFactor = new Parameter(1.0, 1.0, 100, "Positive Dist");
    m_pNegFactor = new Parameter(1.0, 1.0, 100, "Negative Dist");
}

inline double TanhDistortion::InputSample(double sample)
{
    double retVal = 0;
    
    if(m_fPassThrough)
        return sample;
    
    float factor;
    if (sample > 0)
    {
        factor = m_pPosFactor->getValue();
        retVal = tanh(factor*sample)/factor;
    }
    else
    {
        factor = m_pNegFactor->getValue();
        retVal = tanh(factor*sample)/factor;
    }
    
    return retVal * 2;
}

bool TanhDistortion::setPosFactor(double factor)
{
    return m_pPosFactor->setValue(factor);
}

bool TanhDistortion::setNegFactor(double factor)
{
    return m_pNegFactor->setValue(factor);
}

Parameter& TanhDistortion::getPrimaryParam()
{
    return *m_pPosFactor;
}

bool TanhDistortion::setPrimaryParam(float value)
{
    return m_pPosFactor->setValue(value);
}

Parameter& TanhDistortion::getSecondaryParam()
{
    return *m_pNegFactor;
}

bool TanhDistortion::setSecondaryParam(float value)
{
    return m_pNegFactor->setValue(value);
}

TanhDistortion::~TanhDistortion()
{
    delete m_pPosFactor;
    m_pPosFactor = NULL;
    
    delete m_pNegFactor;
    m_pNegFactor = NULL;
}
    

