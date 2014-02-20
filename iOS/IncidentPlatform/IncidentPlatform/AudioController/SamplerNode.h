//
//  SamplerNode.h
//  IncidentPlatform
//
//  Created by Idan Beck on 2/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__SamplerNode__
#define __IncidentPlatform__SamplerNode__

#include "AudioNodeNetwork.h"
#include "dss_list.h"
class SampleNode;

// A sampler is a collection of Sample nodes mixed into
// a bank, and all the banks are mixed into the output
// node.

class SamplerBankNode : public AudioNodeNetwork {
public:
    SamplerBankNode();
    ~SamplerBankNode();
    
    RESULT TriggerSample(int sample);
    RESULT LoadSampleIntoBank(char *pszFilepath, SampleNode *outSample);
    
    SampleNode* operator[](const int& i);
    
public:
    list<SampleNode*> m_samples;
};

class SamplerNode : public AudioNodeNetwork {
public:
    SamplerNode();
    ~SamplerNode();
    
    RESULT TriggerBankSample(int bank, int sample);
    RESULT CreateNewBank(SamplerBankNode *outBank);
    RESULT LoadSampleIntoBank(int bank, char *pszFilepath, SampleNode *outSampleNode);
    
    SamplerBankNode* operator[](const int& i);
    
public:
    list<SamplerBankNode*> m_banks;
};

#endif /* defined(__IncidentPlatform__SamplerNode__) */
