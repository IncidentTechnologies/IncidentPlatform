#include "SoundCmn.h"

#include "math.h"

// This will generate a sine of a specific frequency using the designated SamplingFrequency and Time Length
// Time is in seconds and SamplingFrequency in Hertz
RESULT GenerateSine(float LengthTime, int SamplingFrequency, float SineFrequency, float* &r_rgWaveform, long &r_rgWaveform_n)
{
    RESULT r = R_OK;
    
    CBRM(r_rgWaveform != NULL, "GenerateSine: Input Waveform buffer is not null");

    r_rgWaveform_n = LengthTime * SamplingFrequency;
    r_rgWaveform = new float[r_rgWaveform_n];

    for(long i = 0; i < r_rgWaveform_n; i++)    
    {
        float omega = (float)(SineFrequency * i) / (float)SamplingFrequency;
        omega = omega * 2 * 4.0*atan(1.0);
        r_rgWaveform[i] = sin(omega);    

        //printf("i: %d f: %f\n", i, r_rgWaveform[i]);
    }

Error:
    return r;
}

RESULT GenerateSaw(float LengthTime, int SamplingFrequency, float SawFrequency, float* &r_rgWaveform, long &r_rgWaveform_n)
{
    RESULT r = R_OK;

    CBRM(r_rgWaveform != NULL, "GenerateSine: Input Waveform buffer is not null");

    r_rgWaveform_n = LengthTime * SamplingFrequency;
    r_rgWaveform = new float[r_rgWaveform_n];

    for(long i = 0; i < r_rgWaveform_n; i++)
    {
        r_rgWaveform[i] = 2.0f * ((float)((int)(i * SawFrequency) % SamplingFrequency) / (float)SamplingFrequency) - 1.0f;
        //printf("i: %d f: %f\n", i, r_rgWaveform[i]);
    }

Error:
    return r;
}