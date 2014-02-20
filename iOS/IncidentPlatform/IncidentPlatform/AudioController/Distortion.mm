#include "Distortion.h"

Distortion::Distortion (double gain, double wet, double SamplingFrequency) :
    Effect("Distortion", wet, SamplingFrequency)
{
    m_pDrive = new Parameter(defaultDrive, 1.0f, 20.0f, "Drive");
    m_pFuzzGain = new Parameter(defaultFuzz, 1.0f, 20.0f, "Fuzz");
    
    m_gain = gain;
    m_pTanhDistortion = new TanhDistortion(1.0, 1.0, SamplingFrequency);
    setPrimaryParam(m_pDrive->getValue());
    m_pFuzzExpDistortion = new FuzzExpDistortion(m_pFuzzGain->getValue(), 1.0, SamplingFrequency);
}

inline double Distortion::InputSample(double sample)
{
    if (m_fPassThrough)
        return sample;
    
    float tempSample = 0.0f;
    tempSample = m_pTanhDistortion->InputSample(sample);
    tempSample = tempSample * m_gain;
    tempSample = m_pFuzzExpDistortion->InputSample(tempSample);
    
    tempSample = (1.0f - m_pWet->getValue()) * (sample) + (m_pWet->getValue()) * (tempSample);
    
    return tempSample;
}

Parameter& Distortion::getPrimaryParam()
{
    return *m_pDrive;
}

// This param is the negDistortion param of the TanhDistortion unit. Here we
// will also set the posDistortion param based on the neg value.
bool Distortion::setPrimaryParam(float value)
{
    m_pDrive->setValue(value);
    // from testing we have come up with this formula for relating the
    // posDist param from a given negDist param for the tanh dist
    // posDist = 3.66*negDist - 2.66
    m_pTanhDistortion->setPrimaryParam(3.66f * value - 2.66f);
    return m_pTanhDistortion->setSecondaryParam(value);
}

Parameter& Distortion::getSecondaryParam()
{
    return *m_pFuzzGain;
}

bool Distortion::setSecondaryParam(float value)
{
    m_pFuzzGain->setValue(value);
    return m_pFuzzExpDistortion->SetGain(value);
}

void Distortion::Reset()
{
    Effect::Reset();
    setPrimaryParam(defaultDrive);
    setSecondaryParam(defaultFuzz);
}

Distortion::~Distortion()
{
    delete m_pDrive;
    m_pDrive = NULL;
    
    delete m_pFuzzGain;
    m_pFuzzGain = NULL;
    
    delete m_pTanhDistortion;
    m_pTanhDistortion = NULL;
    
    delete m_pFuzzExpDistortion;
    m_pFuzzExpDistortion = NULL;
}
