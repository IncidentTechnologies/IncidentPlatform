#include "Compressor.h"

Compressor::Compressor(double threshold, double ratio, double attackMs, double releaseMs, int sampleRate )
{
    m_threshold = threshold;
    m_ratio = ratio;
    e.Setup( attackMs, releaseMs, sampleRate );
}

inline void Compressor::Setup( double threshold, double ratio, double attackMs, double releaseMs, int sampleRate )
{
    m_threshold = threshold;
    m_ratio = ratio;
    e.Setup( attackMs, releaseMs, sampleRate );
}
