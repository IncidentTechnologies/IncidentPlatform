//
//  AudioNodeNetwork.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 2/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "AudioNodeNetwork.h"

AudioNodeNetwork::AudioNodeNetwork() :
    AudioNode(),
    m_inputNode(NULL),
    m_outputNode(NULL),
    m_cursorNode(NULL)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    // Create output node with one output channel
    m_outputNode = new AudioNode();
    m_outputNode->SetChannelCount(1, CONN_OUT);
    m_outputNode->SetChannelCount(1, CONN_IN);
    this->ConnectInput(0, m_outputNode, 0);             // Connect our ouput node to the output of the network
    
    // Input?
}

AudioNodeNetwork::~AudioNodeNetwork() {
    if(m_outputNode != NULL) {
        delete m_outputNode;
        m_outputNode = NULL;
    }
    
    if(m_inputNode != NULL) {
        delete m_inputNode;
        m_inputNode = NULL;
    }
}

// Ping output node to get the next sample
float AudioNodeNetwork::GetNextSample(unsigned long int timestamp) {
    if(m_outputNode == NULL){
        return 0.0;
    }
    return m_outputNode->GetNextSample(timestamp);
}