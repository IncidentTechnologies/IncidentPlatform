//
//  RMSNode.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/19/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__LevelNode__
#define __IncidentPlatform__LevelNode__

#include <iostream>
#include "AudioNode.h"
#include "dss_list.h"
#include "CircularBuffer.hxx"

#define DEFAULT_LEVEL_MS_INTERVAL 35

typedef void (*LevelCallback)(float, void*, void*);

typedef enum LEVEL_TYPE {
    LEVEL_RMS,
    LEVEL_ABS_MEAN,
    LEVEL_ABS_GEOMETRIC_MEAN,
    LEVEL_INVALID
} LevelType;

typedef struct Level_SUBSCRIBER {
    void *pObject;
    LevelCallback cb;
    void *pContext;
    LevelType type;
} LevelSubscriber;

using namespace dss;

class LevelNode : public AudioNode {
public:
    LevelNode();
    ~LevelNode();
    
    float GetRMSValue();
    float GetAbsoluteMeanValue();
    float GetAbsoluteGeometricMean();
    float GetValue();
    
    RESULT SetInterval(float msInterval);
    float GetNextSample(unsigned long int timestamp);
    
    RESULT Subscribe(LevelType type, void *pObject, LevelCallback cbLevel, void *pContext);

private:
    RESULT NotifySubscribers();
    
private:
    float m_msNotificationCounter;
    
    float m_msNotificationInterval;
    
    list<LevelSubscriber> m_susbscribers;
    CircularBuffer<float> *m_CircularBuffer;
};

#endif /* defined(__IncidentPlatform__LevelNode__) */
