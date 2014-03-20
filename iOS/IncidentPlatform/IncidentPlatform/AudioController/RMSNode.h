//
//  RMSNode.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__RMSNode__
#define __IncidentPlatform__RMSNode__

#include <iostream>
#include "AudioNode.h"
#include "dss_list.h"
#include "CircularBuffer.hxx"

#define DEFAULT_RMS_MS_INTERVAL 20

typedef void (*RMSCallback)(float, void*, void*);

typedef struct RMS_SUBSCRIBER {
    void *pObject;
    RMSCallback cb;
    void *pContext;
} RMSSubscriber;

using namespace dss;

class RMSNode : public AudioNode {
public:
    RMSNode();
    ~RMSNode();
    
    float GetValue();
    
    RESULT SetInterval(float msInterval);
    float GetNextSample(unsigned long int timestamp);
    
    RESULT Subscribe(void *pObject, RMSCallback cbRMS, void *pContext);

private:
    RESULT NotifySubscribers();
    
private:
    float m_msNotificationCounter;
    
    float m_rms;
    float m_msNotificationInterval;
    
    list<RMSSubscriber> m_susbscribers;
    CircularBuffer<float> *m_CircularBuffer;
};

#endif /* defined(__IncidentPlatform__RMSNode__) */
