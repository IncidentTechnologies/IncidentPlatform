//
//  EffectNode.cpp
//  IncidentPlatform
//
//  Created by Idan Beck on 2/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "EffectNode.h"

EffectNode::EffectNode() :
    AudioNode(),
    m_fPassThrough(false)
{
    m_pWet = new Parameter(1.0f, 0.0f, 1.0f, "Wet");        // Fix later?
}

EffectNode::~EffectNode()
{
    /* empty stub */
}

bool EffectNode::SetPassThru(bool state)
{
    m_fPassThrough = state;
    
    if (state) {
        ClearOutEffect();
    }
    
    return true;
}

bool EffectNode::SetWet(float wet) {
    return m_pWet->setValue(wet);
}


float EffectNode::GetWet() {
    return m_pWet->getValue();
}

void EffectNode::Reset() {
    m_fPassThrough = true;
}

void EffectNode::ClearOutEffect() {
    /* empty stub */
}

bool EffectNode::IsOn() {
    return !m_fPassThrough;
}
