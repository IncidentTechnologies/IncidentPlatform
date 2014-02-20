//
//  EffectNode.h
//  IncidentPlatform
//
//  Created by Idan Beck on 2/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__EffectNode__
#define __IncidentPlatform__EffectNode__

#include "Parameter.h"
#include "AudioNode.h"

class EffectNode : public AudioNode {
public:
    EffectNode();
    ~EffectNode();
    
    bool SetPassThru(bool state);
    bool SetWet(float wet);
    
    float GetWet();
    bool IsOn();
    
    virtual void Reset();
    virtual void ClearOutEffect();
    
protected:
    Parameter *m_pWet;
    bool m_fPassThrough;
};

#endif /* defined(__IncidentPlatform__EffectNode__) */
