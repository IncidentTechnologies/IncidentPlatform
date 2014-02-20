#pragma once

#include <math.h>
#include "RESULT.h"
#include "EHM.h"

// This will generate a sine of a specific frequency using the designated SamplingFrequency and Time Length
// Time is in seconds and SamplingFrequency in Hertz
RESULT GenerateSine(float LengthTime, int SamplingFrequency, float SineFrequency, float* &r_rgWaveform, long &r_rgWaveform_n);

RESULT GenerateSaw(float LengthTime, int SamplingFrequency, float SineFrequency, float* &r_rgWaveform, long &r_rgWaveform_n);