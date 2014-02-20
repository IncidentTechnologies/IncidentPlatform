//
//  WavetableOscillatorNode.h
//  AudioController
//
//  Created by Idan Beck on 2/12/14.
//
//

#ifndef IncidentPlatform_WavetableNode_h
#define IncidentPlatform_WavetableNode_h

#include <math.h>
#include "GeneratorNode.h"

#define WAVETABLE_LENGTH 44100

typedef float (*fnWavetable)(float);

float WavetableSine(float theta);
float WavetableSaw(float theta);
float WavetableSquare(float theta);
float WavetableTriangle(float theta);
float WavetableInvalid(float theta);

typedef enum {
    WAVETABLE_SINE = 0,
    WAVETABLE_SAW = 1,
    WAVETABLE_SQUARE = 2,
    WAVETABLE_TRIANGLE = 3,
    WAVETABLE_INVALID
} WAVETABLE_TYPE;

extern fnWavetable WavetableFunctions[];

class WavetableNode : public GeneratorNode {
public:
    WavetableNode();
    
    float GetNextSample(unsigned long int timestamp);
    
    void trigger();
    
    float SetFrequency(float freq);
    float SetPhaseOffset(float phase);

private:
    void InitializeWavetable();
    
public:
    WAVETABLE_TYPE m_type;
    float m_phase;
    float m_frequency;
    float m_theta;
    
private:
    float m_wavetable[WAVETABLE_LENGTH];
};

#endif