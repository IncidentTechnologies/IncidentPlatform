#include "KSObject.h"


KSObject::KSObject() :
    m_BufferKS(NULL),
    m_eBufferKS(0),
    m_FreqKS(MIN_FREQ),
    m_Fs(44100.0f),
    m_attenuationKS(0.99f),
    m_currentAttenuation(m_attenuationKS),
    m_attenuationVariation(0),
    m_pBWFilter(NULL),
    m_bwFltOrder(10),
    m_bwFltCutoff(2000),
    m_b3rdHarmonicOn(true),
    m_b5thHarmonicOn(false),
    m_noiseScale(0.3f),
    m_noiseVariation(0.3f),
    m_bSawToothOn(true),
    m_sawFreqMultiplier(1),
    m_bSqOn(true),
    m_sqFreqMultiplier(2)
{
    m_BufferKS_n = (int) m_Fs / MIN_FREQ;
    m_BufferKS = new float[m_BufferKS_n];
    memset(m_BufferKS, 0, sizeof(float) * m_BufferKS_n);
    m_BufferKS_CurLength = (int) (m_Fs / m_FreqKS);
    
    // Set up the butterworth filter
    if(m_bwFltOrder > 0)
        m_pBWFilter = new ButterWorthFilter(m_bwFltOrder, m_bwFltCutoff, m_Fs);
    else
        m_pBWFilter = NULL;
}
	
KSObject::KSObject(float SamplingFreq) :
    m_BufferKS(NULL),
    m_eBufferKS(0),
    m_FreqKS(420),
    m_Fs(SamplingFreq),
    m_attenuationKS(0.99f),
    m_currentAttenuation(m_attenuationKS),
    m_attenuationVariation(0),
    m_pBWFilter(NULL),
    m_bwFltOrder(10),
    m_bwFltCutoff(2000),
    m_b3rdHarmonicOn(true),
    m_b5thHarmonicOn(false),
    m_noiseScale(0.3f),
    m_noiseVariation(0.3f),
    m_bSawToothOn(true),
    m_sawFreqMultiplier(1),
    m_bSqOn(true),
    m_sqFreqMultiplier(2)
{
    m_BufferKS_n = (int) m_Fs / MIN_FREQ;
    m_BufferKS = new float[m_BufferKS_n];
    memset(m_BufferKS, 0, sizeof(float) * m_BufferKS_n);
    
    // Set up the butterworth filter
    if(m_bwFltOrder > 0)
        m_pBWFilter = new ButterWorthFilter(m_bwFltOrder, m_bwFltCutoff, m_Fs);
    else
        m_pBWFilter = NULL;
}
	
/*
 freq - freqency of note to pluck
 pluckAmplitude - factor indicating how strongly the string was plucked, in range [0,1]
 */
void KSObject::Pluck(float freq, float pluckAmplitude)
{
    // set up an example pluck
    m_FreqKS = freq;
    m_BufferKS_CurLength = (int) (m_Fs / m_FreqKS);
    m_eBufferKS = m_BufferKS_CurLength - 1;
    
    // set up phase and phase increments for each of the oscillators
    float phase = 0;
    float phase3rdHarmonic = 0;
    float phase5thHarmonic = 0;
    float phaseSawTooth = 0;
    float phaseSq = 0;
    float phaseIncrement = (freq/2.0f) * (2 * M_PI) / m_Fs;
    float phaseInc3rdHarmonic = (3.0f * freq/2.0f) * (2 * M_PI) / m_Fs;
    float phaseInc5thHarmonic = (5.0f * freq/2.0f) * (2 * M_PI) / m_Fs;
    float phaseSawInc = (m_sawFreqMultiplier * freq/2.0f) * (2 * M_PI) / m_Fs;
    float phaseSqInc = (m_sqFreqMultiplier*freq/2.0f) * (2 * M_PI) / m_Fs;
    
    int volatile numOsc = 0;
    if (m_b3rdHarmonicOn)
        numOsc++;
    if (m_bSawToothOn)
        numOsc++;
    if (m_bSqOn)
        numOsc++;
    
    float volatile tmpVal;
    float randFloat = (float)rand() / (float)RAND_MAX;      // range of [0, 1]
    float randFloatAdj = (randFloat - 0.5f) * 2.0f;         // converted to a range of [-1, 1]
    float noiseVariation = m_noiseVariation * randFloatAdj; // range of [-m_noiseVariation, m_noiseVariation]
    float currentNoiseScale = m_noiseScale + m_noiseScale * noiseVariation;
    
    // initialize the delay line buffer
    for(int i = 0; i < m_BufferKS_CurLength; i++)
    {
        tmpVal = 0;
        
        // Sine wave plus it's 3rd harmonic
        if (m_b3rdHarmonicOn)
        {
            tmpVal = sin(phase);
            tmpVal += sin(phase3rdHarmonic)/2;
        }
        // sine wave's 5th order harmonic
        if (m_b5thHarmonicOn)
        {
            tmpVal += sin(phase5thHarmonic)/4;
        }
        
        // Saw tooth wave
        if (m_bSawToothOn)
        {
            tmpVal += ((phaseSawTooth / (2 * M_PI)) - 0.5f) * 2.0f;
        }
        
        // Square wave
        if (m_bSqOn)
        {
            if(phaseSq < M_PI)
                tmpVal += 1.0f;
            else
                tmpVal += -1.0f;
        }
        
        // scale down sum so output is in range [-1, 1]
        if (numOsc > 0)
        {
            tmpVal = tmpVal/numOsc;
        }
        
        // calculate the next phase
        phase += phaseIncrement;
        phase3rdHarmonic += phaseInc3rdHarmonic;
        phase5thHarmonic += phaseInc5thHarmonic;
        phaseSawTooth += phaseSawInc;
        phaseSq += phaseSqInc;
        
        // adjust phase if > 2pi to prevent overflow
        if(phase >= 2 * M_PI)
            phase = phase - 2 * M_PI;
        if(phase3rdHarmonic >= 2 * M_PI)
            phase3rdHarmonic = phase3rdHarmonic - 2 * M_PI;
        if(phase5thHarmonic >= 2 * M_PI)
            phase5thHarmonic = phase5thHarmonic - 2 * M_PI;
        if(phaseSawTooth >= 2 * M_PI)
            phaseSawTooth = phaseSawTooth - 2 * M_PI;
        if(phaseSq >= 2 * M_PI)
            phaseSq = phaseSq - 2 * M_PI;
        
        // add noise
        randFloat = (float)rand() / (float)RAND_MAX;      // range of [0, 1]
        randFloatAdj = (randFloat - 0.5f) * 2.0f;         // converted to a range of [-1, 1]
        tmpVal += currentNoiseScale * randFloatAdj;       // range of [-currentNoiseScale, currentNoiseScale]
        
        // scale down to pluckAmplitude
        tmpVal *= pluckAmplitude;
        m_BufferKS[i] = tmpVal;
    }
    
    // Setup attenuation for current pluck
    float attenuationVariation = randFloatAdj * m_attenuationVariation; // range of [-m_attenuationVariation, m_attenuationVariation]
    m_currentAttenuation = m_attenuationKS + m_attenuationKS * attenuationVariation;
    if (m_currentAttenuation > 0.9999f)
    {
        m_currentAttenuation = 0.9999f;
    }
}

bool KSObject::SetBWFilterCutoff(double cutoff)
{
    m_bwFltCutoff = cutoff;
    if(m_pBWFilter != NULL)
        return m_pBWFilter->SetCutoff(m_bwFltCutoff);
    else
        return false;
}

bool KSObject::SetBWFilterOrder(int order)
{
    m_bwFltOrder = order;
    if(m_pBWFilter != NULL)
        return m_pBWFilter->SetOrder(order);
    else
        return false;
}

/*inline */double KSObject::GetNextKSSample()
{
    // first ensure that the pluck is initialized
    // TODO can we remove this check? its costing us ~1%
    if(m_BufferKS == NULL || m_FreqKS == 0)
        return 0;
    
    double CurVal = m_BufferKS[m_eBufferKS];
    
    // TODO
    // This conditional is costing us ~2%, remove it.
    int order;
    if(m_bwFltOrder == 0)
        order = 2;
    else
        order = m_bwFltOrder;
    
    float pValArray[MAX_BUTTERWORTH_ORDER];
    
    for(int i = 0; i < order; i++)
    {
        int SampleIndex = 0;
        if(m_eBufferKS + i >= m_BufferKS_CurLength)
            SampleIndex = m_eBufferKS + i - m_BufferKS_CurLength;
        else
            SampleIndex = m_eBufferKS + i;
        
        if(SampleIndex < m_BufferKS_CurLength)
            pValArray[i] = m_BufferKS[SampleIndex];
        else
        {
            // TODO
            // This condition is sometimes triggering and the NSlog is costing ~%0.6 for no benefit
            NSLog(@"err: Out of bounds in KS array lookup!");
            pValArray[i] = 0;
        }
    }
    
    float NewValf;
    if(m_bwFltOrder == 0 || m_pBWFilter == NULL)
        NewValf = m_currentAttenuation * (0.5 * pValArray[0] + 0.5 * pValArray[1]);
    else
        NewValf = m_currentAttenuation * m_pBWFilter->InputSample(CurVal);
    
    // Now replace the current sample with the filtered one
    m_BufferKS[m_eBufferKS] = NewValf;
    
    m_eBufferKS++;
    if(m_eBufferKS >= m_BufferKS_CurLength)
        m_eBufferKS = 0;
    
    return CurVal;
}

void KSObject::Set3rdOrderHarmonicOn(bool on)
{
    m_b3rdHarmonicOn = on;
}

void KSObject::Set5thOrderHarmonicOn(bool on)
{
    m_b5thHarmonicOn = on;
}

bool KSObject::SetNoiseScale(float scale)
{
    m_noiseScale = scale;
    return true;
}

bool KSObject::SetNoiseVariation(float variation)
{
    if (variation < 0 || variation > 1.0)
    {
        return false;
    }
    m_noiseVariation = variation;
    return true;
}

void KSObject::SetSawToothOn(bool on)
{
    m_bSawToothOn = on;
}

bool KSObject::SetSawMultiplier(float multiplier)
{
    m_sawFreqMultiplier = multiplier;
    return true;
}

void KSObject::SetSqWaveOn(bool on)
{
    m_bSqOn = on;
}

bool KSObject::SetSqMultiplier(float multiplier)
{
    m_sqFreqMultiplier = multiplier;
    return true;
}

bool KSObject::SetAttenuationVariation(float variation)
{
    if (variation < 0 || variation > 1.0)
    {
        return false;
    }
    m_attenuationVariation = variation;
    return true;
}

KSObject::~KSObject()
{
    delete [] m_BufferKS;
    m_BufferKS = NULL;
    
    delete m_pBWFilter;
    m_pBWFilter = NULL;
}

float KSObject::GuitarFreqLookup(int string, int fret)
{
    int midi = 40 + string * 5;
    if(string > 3)
        midi -= 1;
    
    midi += fret;
    
    // Now we have the midi note we can get the frequency
    // f = Fr * 2^(midi / 12)
    float f = 440.0f * pow(2.0, (float)(midi - 69) / 12.0f);
    
    return f;
}

