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

typedef void (*RMSCallback)(float, void*);

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
    
private:
    float m_rms;
    list<RMSSubscriber> m_cbSusbscriber;
};

#endif /* defined(__IncidentPlatform__RMSNode__) */
