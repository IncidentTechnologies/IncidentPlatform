//
//  GtarSamplerNode.h
//  IncidentPlatform
//
//  Created by Kate Schnippering on 5/13/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__GtarSamplerNode__
#define __IncidentPlatform__GtarSamplerNode__

#define NUM_BANKS 1

#include "AudioNodeNetwork.h"
#include "dss_list.h"
#include "SampleNode.h"
#include "SamplerNode.h"

class SampleNode;

// A sampler is a collection of Sample nodes mixed into
// a bank, and all the banks are mixed into the output
// node.

using namespace dss;

class GtarSamplerNode : public GeneratorNode {
public:
    GtarSamplerNode();
    ~GtarSamplerNode();
    
    float GetNextSample(unsigned long int timestamp);
    
    RESULT ReleaseBank();
    RESULT CreateNewBank(int numSamples);
    RESULT TriggerSample(int sample);
    RESULT StopSample(int sample);
    
    RESULT LoadSampleIntoBank(char *pszFilepath);
    
public:
    SampleBuffer **m_ppSampleBuffers;
    bool m_fPlaying;
    int m_numSamples;
    int m_nextSampleCounter;
    //SamplerBankNode *m_samplerBankNode;
    
};
 

#endif /* defined(__IncidentPlatform__SamplerNode__) */
