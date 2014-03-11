//
//  SamplerNode.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 2/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "SamplerNode.h"
#include "SampleNode.h"

/******************************/
// Sampler Bank Node
/******************************/

SamplerBankNode::SamplerBankNode() :
    AudioNodeNetwork()
{
    /* empty stub */
}

SamplerBankNode::~SamplerBankNode() {
    while(m_samples.length() > 0) {
        SampleNode *tempNode = m_samples.Pop();
        delete tempNode;
        tempNode = NULL;
    }
}

RESULT SamplerBankNode::TriggerSample(int sample) {
    RESULT r = R_SUCCESS;
    
    CBRM((sample < m_samples.length()), "SamplerBankNode: Not that many samples!");
    m_samples[sample]->Trigger();
    
Error:
    return r;
}

RESULT SamplerBankNode::SetSampleGain(int sample, float gain) {
    RESULT r = R_SUCCESS;
    
    CBRM((sample < m_samples.length()), "SamplerBankNode: Not that many samples!");
    m_samples[sample]->SetChannelGain(gain, CONN_OUT);
    
Error:
    return r;
}

RESULT SamplerBankNode::SetBankGain(float gain) {
    this->SetChannelGain(gain, CONN_OUT);
    return R_SUCCESS;
}

SampleNode* SamplerBankNode::GetSample(int sample) {
    if(sample < m_samples.length())
        return m_samples[sample];
    else
        return NULL;
}

RESULT SamplerBankNode::LoadSampleIntoBank(char *pszFilepath, SampleNode* &outSample) {
    RESULT r = R_SUCCESS;
    
    SampleNode *newSample = new SampleNode(pszFilepath);
    
    // Check, connect, and push the new sample
    CNRM(newSample, "SamplerBankNode: Failed to create sample");
    CRM(m_outputNode->ConnectInput(0, newSample, 0), "SamplerBankNode: Failed to connect new sample node to output");
    CRM(m_samples.Append(newSample), "SamplerBankNode: Failed to add sample to bank");
    
    // pass out the sample
    outSample = newSample;
    
    return r;
    
Error:
    
    // Gotta watch fails hard here (big memory leaks possible)
    if(newSample != NULL) {
        delete newSample;
        newSample = NULL;
    }
    return r;
}

/*
float SamplerBankNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    return m_outputNode->GetNextSample(timestamp);
}*/

SampleNode*& SamplerBankNode::operator[](const int& i) {
    SampleNode *retVal = NULL;
    
    if(i < m_samples.length())
        retVal = m_samples[i];
    
    return retVal;
}

/******************************/
// Sampler Node
/******************************/

SamplerNode::SamplerNode() :
    AudioNodeNetwork()
{
    /* empty stub */
}

SamplerNode::~SamplerNode() {
    while(m_banks.length() > 0) {
        SamplerBankNode *tempNode = m_banks.Pop();
        delete tempNode;
        tempNode = NULL;
    }
}

RESULT SamplerNode::ReleaseBank(SamplerBankNode* &bank) {
    RESULT r = R_SUCCESS;
    
    CNRM(bank, "ReleaseBank: Bank to release is NULL");
    CRM(m_banks.Remove(bank, GET_BY_ITEM), "ReleaseBank: Bank does not exist in sampler");
    
    if(bank != NULL) {
        delete bank;
        bank = NULL;
    }
    
Error:
    return r;
}

RESULT SamplerNode::ReleaseBank(int bank) {
    RESULT r = R_SUCCESS;
    
    SamplerBankNode *node = NULL;
    CRM(m_banks.Remove(node, (void*)(&bank), GET_BY_POSITION), "SamplerNode: Failed to release bank %d", bank);
    CNRM(node, "SampleNode: Failed to acquire bank %d", bank);
    
    if(node != NULL) {
        delete node;
        node = NULL;
    }
    
Error:
    return r;
}

RESULT SamplerNode::CreateNewBank(SamplerBankNode* &outBank){
    RESULT r = R_SUCCESS;
    
    SamplerBankNode *newBank = new SamplerBankNode();
    CNRM(newBank, "SamplerNode: Failed to allocate new bank");
    CRM(m_outputNode->ConnectInput(0, newBank, 0), "SamplerNode: Failed to connect bank output to Sampler output");
    CRM(m_banks.Append(newBank), "SamplerNode: Failed to add bank to sampler");
    
    // Pass it out
    outBank = newBank;
    
Error:
    return r;
}

RESULT SamplerNode::LoadSampleIntoBank(int bank, char *pszFilepath, SampleNode* &outSampleNode) {
    RESULT r = R_SUCCESS;
    SamplerBankNode *tempBank = NULL;
    
    CBRM((bank < m_banks.length()), "SamplerNode: Can't add sample to non-existent bank");
    
    tempBank = m_banks[bank];
    
    CRM(tempBank->LoadSampleIntoBank(pszFilepath, outSampleNode), "SamplerNode: Failed to add sample with path %s", pszFilepath);
    
Error:
    return r;
}

RESULT SamplerNode::TriggerBankSample(int bank, int sample) {
    RESULT r = R_SUCCESS;
    
    CBRM((bank < m_banks.length()), "SamplerNode: Not that many banks!");
    CRM(m_banks[bank]->TriggerSample(sample), "SamplerNode: Failed to trigger sample in bank");
    
Error:
    return r;
}

RESULT SamplerNode::SetBankGain(int bank, float gain) {
    RESULT r = R_SUCCESS;
    
    CBRM((bank < m_banks.length()), "SamplerNode: Not that many banks!");
    m_banks[bank]->SetChannelGain(gain, CONN_OUT);
    
Error:
    return r;
}

SampleNode* SamplerNode::GetBankSample(int bank, int sample) {
    if(bank < m_banks.length())
        return m_banks[bank]->GetSample(sample);
    else
        return NULL;
}

SamplerBankNode*& SamplerNode::operator[](const int& i) {
    SamplerBankNode *retVal = NULL;
    
    if(i < m_banks.length())
        retVal = m_banks[i];
    
    return retVal;
}

/*
float SamplerNode::GetNextSample(unsigned long int timestamp) {
    float retVal = 0.0f;
    
    return retVal;
}
 */


