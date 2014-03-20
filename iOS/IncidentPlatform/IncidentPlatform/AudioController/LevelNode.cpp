//
//  RMSNode.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 3/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "LevelNode.h"
#include "CircularBuffer.hxx"
#include <math.h>

LevelNode::LevelNode() :
    m_CircularBuffer(NULL),
    m_msNotificationInterval(DEFAULT_LEVEL_MS_INTERVAL),
    m_msNotificationCounter(0.0f)
{
    SetChannelCount(1, CONN_OUT);
    SetChannelCount(1, CONN_IN);
    
    m_CircularBuffer = new CircularBuffer<float>(DEFAULT_LEVEL_MS_INTERVAL, m_SampleRate);
}

LevelNode::~LevelNode() {
    if(m_CircularBuffer != NULL) {
        delete m_CircularBuffer;
        m_CircularBuffer = NULL;
    }
}

float LevelNode::GetRMSValue() {
    double retVal = 0.0f;
    
    for(unsigned long i = 0 ; i < m_CircularBuffer->length(); i++) {
        float val = (float)(*m_CircularBuffer)[i];
        retVal += powf(val, 2.0f) / (float)(m_CircularBuffer->length());
    }
    
    return sqrt(retVal);
}

float LevelNode::GetAbsoluteMeanValue() {
    double retVal = 0.0f;
    
    for(unsigned long i = 0 ; i < m_CircularBuffer->length(); i++) {
        float val = (float)(*m_CircularBuffer)[i];
        retVal += fabsf(val) / (double)(m_CircularBuffer->length());
    }
    
    return retVal;
}

float LevelNode::GetAbsoluteGeometricMeanValue() {
    double retVal = 0.0f;
    
    for(unsigned long i = 0 ; i < m_CircularBuffer->length(); i++) {
        float val = (float)(*m_CircularBuffer)[i];
        
        retVal *= pow(fabsf(val), 1.0f/(double)(m_CircularBuffer->length()));
    }
    
    return retVal;
}

RESULT LevelNode::SetInterval(float msInterval) {
    RESULT r = R_SUCCEED;
    
    m_msNotificationInterval = msInterval;
    CRM(m_CircularBuffer->SetLength(msInterval, m_SampleRate), "RMSNode:SetInterval falied on SetLength on Circular Buffer");
    
Error:
    return r;
}

RESULT LevelNode::NotifySubscribers() {
    // niether of these can be negative
    float rmsValue = -1;// = GetRMSValue();
    float absMeanValue = -1;
    float absGeoMeanValue = -1;

    m_msNotificationCounter = 0.0f;
    
    for(list<LevelSubscriber*>::iterator it = m_susbscribers.First(); it != NULL; it++) {
        float value;
        
        switch((*it)->type) {
            case LEVEL_RMS: {
                if(rmsValue < 0)
                    rmsValue = GetRMSValue();
                value = rmsValue;
            } break;
                
            case LEVEL_ABS_MEAN: {
                if(absMeanValue < 0)
                    absMeanValue = GetAbsoluteMeanValue();
                value = absMeanValue;
            } break;
            
            case LEVEL_ABS_GEOMETRIC_MEAN: {
                if(absGeoMeanValue < 0)
                    absGeoMeanValue = GetAbsoluteGeometricMeanValue();
                value = absGeoMeanValue;
            }
                
            case LEVEL_INVALID: {
                value = 0.0f;
            } break;
        }
        
        (*it)->cb(value, (*it)->pObject, (*it)->pContext);
    }
    
    return R_SUCCEED;
}

float LevelNode::GetNextSample(unsigned long int timestamp) {
    float retVal = AudioNode::GetNextSample(timestamp);
    
    m_CircularBuffer->PushElement(retVal);
    m_msNotificationCounter += (1000.0f/m_SampleRate);
    
    if(m_msNotificationCounter > m_msNotificationInterval) {
        NotifySubscribers();
    }
    
    return retVal;
}

RESULT LevelNode::Subscribe(LevelType type, void *pObject, LevelCallback cbLevel, void *pContext) {
    RESULT r = R_SUCCEED;
    
    LevelSubscriber *sub = new LevelSubscriber;
    memset(sub, 0, sizeof(LevelSubscriber));
    
    sub->type = type;
    sub->pContext = pContext;
    sub->pObject = pObject;
    sub->cb = cbLevel;
    
    CRM(m_susbscribers.Append(sub), "RMSNode: Subscribe faield to append subscriber");;
    
Error:
    return r;
}

RESULT LevelNode::UnSubscribe(void *pObject) {
    RESULT r = R_SUCCESS;
    
    for(list<LevelSubscriber*>::iterator it = m_susbscribers.First(); it != NULL; it++) {
        if((*it)->pObject == pObject) {
            LevelSubscriber *tempSub = NULL;
            CRM(m_susbscribers.Remove(tempSub, (void*)(*it), GET_BY_ITEM), "UnSubscribe failed to remove subscriber");
            
            if(tempSub != NULL) {
                delete tempSub;
                tempSub = NULL;
            }
            
            // dont break since might be doubly subscribed
        }
    }
    
Error:
    return r;
}

