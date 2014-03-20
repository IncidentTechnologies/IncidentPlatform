//
//  RMSNode.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 3/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "RMSNode.h"
#include "CircularBuffer.hxx"
#include <math.h>

RMSNode::RMSNode() :
    m_CircularBuffer(NULL),
    m_msNotificationInterval(DEFAULT_RMS_MS_INTERVAL),
    m_msNotificationCounter(0.0f)
{
    SetChannelCount(1, CONN_OUT);
    SetChannelCount(1, CONN_IN);
    
    m_CircularBuffer = new CircularBuffer<float>(DEFAULT_RMS_MS_INTERVAL, m_SampleRate);
}

RMSNode::~RMSNode() {
    if(m_CircularBuffer != NULL) {
        delete m_CircularBuffer;
        m_CircularBuffer = NULL;
    }
}

float RMSNode::GetValue() {
    double retVal = 0.0f;
    
    for(unsigned long i = 0 ; i < m_CircularBuffer->length(); i++) {
        float val = (float)(*m_CircularBuffer)[i];
        retVal += powf((val/((float)m_CircularBuffer->length())), 2.0f);
    }
    
    return sqrt(retVal);
}

RESULT RMSNode::SetInterval(float msInterval) {
    RESULT r = R_SUCCEED;
    
    m_msNotificationInterval = msInterval;
    CRM(m_CircularBuffer->SetLength(msInterval, m_SampleRate), "RMSNode:SetInterval falied on SetLength on Circular Buffer");
    
Error:
    return r;
}

RESULT RMSNode::NotifySubscribers() {
    float value = GetValue();

    m_msNotificationCounter = 0.0f;
    
    for(list<RMSSubscriber>::iterator it = m_susbscribers.First(); it != NULL; it++)
        (*it).cb(value, (*it).pObject, (*it).pContext);
    
    return R_SUCCEED;
}

float RMSNode::GetNextSample(unsigned long int timestamp) {
    float retVal = AudioNode::GetNextSample(timestamp);
    
    m_CircularBuffer->PushElement(retVal);
    m_msNotificationCounter += (1000.0f/m_SampleRate);
    
    if(m_msNotificationCounter > m_msNotificationInterval) {
        NotifySubscribers();
    }
    
    return retVal;
}

RESULT RMSNode::Subscribe(void *pObject, RMSCallback cbRMS, void *pContext) {
    RESULT r = R_SUCCEED;
    
    RMSSubscriber sub;
    memset(&sub, 0, sizeof(RMSSubscriber));
    
    sub.pContext = pContext;
    sub.pObject = pObject;
    sub.cb = cbRMS;
    
    CRM(m_susbscribers.Append(sub), "RMSNode: Subscribe faield to append subscriber");;
    
Error:
    return r;
}

