//
//  ButterWorthFilterNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "ButterWorthFilterNode.h"

ButterWorthFilterNode::ButterWorthFilterNode(int order, double cutoff, double SamplingFrequency) :
//Effect  ("Filter", 1.0, SamplingFrequency),
EffectNode(),
m_ppFilters(NULL),
m_SamplingFrequency(SamplingFrequency)
{
    SetChannelCount(1, CONN_IN);
    SetChannelCount(1, CONN_OUT);
    
    //SetWet(wet);
    
    m_pCutoff = new Parameter(cutoff, 10, 5000, "Frequency");
    m_order = order + (order % 2);  // ensure only even orders
    if(m_order == 0)                // no zero order filters
        m_order = 2;
    
    m_ppFilters_n = m_order / 2;
    
    m_ppFilters = new ButterWorthFilterElementNode*[m_ppFilters_n];
    
    // order is just a cascade of 2nd order filters
    for(int i = 0; i < m_ppFilters_n; i++)
    {
        m_ppFilters[i] = new ButterWorthFilterElementNode(0, 2, m_pCutoff->getValue(), m_SamplingFrequency);
    }
}

// Passes the AudioNode::GetNextSample to InputSample which calculates the next output sample
// and returns it
float ButterWorthFilterNode::GetNextSample(unsigned long int timestamp) {
    float inVal = AudioNode::GetNextSample(timestamp);  // will get the input from all incoming nodes
    return InputSample(inVal);
}


bool ButterWorthFilterNode::SetCutoff(double cutoff)
{
    bool retVal = true;
    
    // propogate the change
    for(int i = 0; i < m_ppFilters_n; i++)
    {
        if(!(retVal = m_ppFilters[i]->CalculateCoefficients(0, cutoff, m_SamplingFrequency)))
            break;
    }
    
    return retVal;
}

inline double ButterWorthFilterNode::InputSample(double sample)
{
    if(m_fPassThrough)
        return sample;
    
    double retVal = sample;
    
    //for(int i = 0; i < m_ppFilters_n; i++)
    
    if((m_order / 2) > m_ppFilters_n)     // error condition!
    {
        return 0;
    }
    
    for(int i = 0; i < (m_order / 2); i++)
        retVal = m_ppFilters[i]->InputSample(retVal);
    
    return retVal;
}

bool ButterWorthFilterNode::SetOrder(int order)
{
    // first of all adjust to ensure it's an even order
    int NewOrder = order + (order % 2);
    
    if(NewOrder == m_order)
        return true;
    
    if((NewOrder / 2) < m_ppFilters_n)
    {
        m_order = NewOrder;
    }
    else
    {
        // We need to create a larger set of filters!
        // delete existing filters
        for(int i = 0; i < m_ppFilters_n; i++)
        {
            if(m_ppFilters[i] != NULL)
            {
                delete m_ppFilters[i];
                m_ppFilters[i] = NULL;
            }
        }
        delete [] m_ppFilters;
        
        // create new ones
        m_order = NewOrder;
        m_ppFilters_n = m_order / 2;
        m_ppFilters = new ButterWorthFilterElementNode*[m_ppFilters_n];
        
        // order is just a cascade of 2nd order filters
        for(int i = 0; i < m_ppFilters_n; i++)
        {
            //m_ppFilters[i] = new ButterWorthFilterElement(i, m_order, m_cutoff, m_SamplingFrequency);
            m_ppFilters[i] = new ButterWorthFilterElementNode(0, 2, m_pCutoff->getValue(), m_SamplingFrequency);
        }
    }
    
    return true;
}

void ButterWorthFilterNode::Reset()
{
    for(int i = 0; i < m_ppFilters_n; i++)
    {
        m_ppFilters[i]->Reset();
    }
}

Parameter* ButterWorthFilterNode::getPrimaryParam()
{
    return m_pCutoff;
}

bool ButterWorthFilterNode::setPrimaryParam(float value)
{
    return SetCutoff(value);
}

Parameter* ButterWorthFilterNode::getSecondaryParam()
{
    return getPrimaryParam();
}

bool ButterWorthFilterNode::setSecondaryParam(float value)
{
    return setPrimaryParam(value);
}

ButterWorthFilterNode::~ButterWorthFilterNode()
{
    if(m_ppFilters != NULL)
    {
        for(int i = 0; i < m_ppFilters_n; i++)
        {
            if(m_ppFilters[i] != NULL)
            {
                delete m_ppFilters[i];
                m_ppFilters[i] = NULL;
            }
        }
        
        delete [] m_ppFilters;
        m_ppFilters = NULL;
    }
    
    delete m_pCutoff;
    m_pCutoff = NULL;
}

