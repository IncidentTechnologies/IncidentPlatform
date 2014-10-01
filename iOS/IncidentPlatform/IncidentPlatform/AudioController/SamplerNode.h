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
#include "LevelNode.h"

class SampleNode;

// A sampler is a collection of Sample nodes mixed into
// a bank, and all the banks are mixed into the output
// node.

using namespace dss;

class SamplerBankNode : public AudioNodeNetwork {
public:
    SamplerBankNode();
    ~SamplerBankNode();
    
    RESULT TriggerSample(int sample);
    RESULT StopSample(int sample);
    RESULT StopAllSamples();
    SampleNode *GetSample(int sample);
    
    RESULT LoadSampleIntoBank(char *pszFilepath, SampleNode* &outSample);
    
    RESULT SetSampleGain(int sample, float gain);
    RESULT SetBankGain(float gain);
    
    RESULT UnSubscribe(LevelSubscriber *pSub);
    LevelSubscriber* SubscribeLevel(LevelType type, void *pObject, LevelCallback cbRMS, void *pContext);
    LevelSubscriber* SubscribeRMS(void *pObject, LevelCallback cbRMS, void *pContext);
    LevelSubscriber* SubscribeAbsoluteMean(void *pObject, LevelCallback cbLevel, void *pContext);
    
public:
    SampleNode*& operator[](const int& i);
    LevelNode *m_levelNode;
    
public:
    dss::list<SampleNode*> m_samples;
};

class SamplerNode : public AudioNodeNetwork {
public:
    SamplerNode();
    ~SamplerNode();
    
    RESULT TriggerBankSample(int bank, int sample);
    SampleNode *GetBankSample(int bank, int sample);
    RESULT StopAllBankSamples();
    
    RESULT ReleaseBank(int bank);
    RESULT ReleaseBank(SamplerBankNode* &bank);
    RESULT RemoveBank(SamplerBankNode *bank);
    
    RESULT CreateNewBank(SamplerBankNode* &outBank);
    RESULT LoadSampleIntoBank(int bank, char *pszFilepath, SampleNode* &outSampleNode);
    
    RESULT SetBankGain(int bank, float gain);
    
    RESULT UnSubscribe(LevelSubscriber *pSub);
    LevelSubscriber* SubscribeLevel(LevelType type, void *pObject, LevelCallback cbRMS, void *pContext);
    LevelSubscriber* SubscribeRMS(void *pObject, LevelCallback cbRMS, void *pContext);
    LevelSubscriber* SubscribeAbsoluteMean(void *pObject, LevelCallback cbLevel, void *pContext);
    
public:
    SamplerBankNode*& operator[](const int& i);
    
public:
    dss::list<SamplerBankNode*> m_banks;
    
    LevelNode *m_levelNode;
};

#endif /* defined(__IncidentPlatform__SamplerNode__) */
