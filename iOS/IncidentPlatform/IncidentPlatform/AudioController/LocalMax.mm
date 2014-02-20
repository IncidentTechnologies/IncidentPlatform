#include "LocalMax.h"

LocalMax::LocalMax(double SamplingFrequency):
    m_SamplingFrequency(SamplingFrequency),
    m_currentSampleIndex(0),
    m_currentMax(0)
{
    // Keep a sample history of 2 wavelengths of the lowest frequency.
    // Lowest freq is open E string, 82.407Hz
    m_buffLen = (int) (2.0 * (1.0f/82.407) * m_SamplingFrequency);
    m_pSampleBuffer = new double[m_buffLen];
    memset(m_pSampleBuffer, 0, sizeof(double) * m_buffLen);
    m_currentMaxAge = m_buffLen;
}

double LocalMax::GetLocalMax(double sample)
{
    m_pSampleBuffer[m_currentSampleIndex] = sample;
    
    if (m_currentMaxAge >= m_buffLen)
    {
        float localMax = 0;
        // calculate max over last m_buffLenSamples;
        for (int i = 0; i < m_buffLen; i++)
        {
            float abs = fabs(m_pSampleBuffer[i]);
            if (abs > localMax)
            {
                localMax = abs;
            }
        }
        m_currentMax = localMax;
        m_currentMaxAge = 0;
    }
    
    if (sample > m_currentMax)
    {
        m_currentMax = sample;
        m_currentMaxAge = 0;
    }
    
    m_currentSampleIndex++;
    if (m_currentSampleIndex >= m_buffLen)
    {
        m_currentSampleIndex = 0;
    }
    
    m_currentMaxAge++;
    
    return m_currentMaxAge;
}

LocalMax::~LocalMax()
{
    delete m_pSampleBuffer;
    m_pSampleBuffer = NULL;
}
    

