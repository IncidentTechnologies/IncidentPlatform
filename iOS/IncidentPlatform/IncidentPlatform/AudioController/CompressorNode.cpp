//
//  CompressorNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "CompressorNode.h"

CompressorNode::CompressorNode(double threshold, double ratio, double attackMs, double releaseMs, int sampleRate )
{
    m_threshold = threshold;
    m_ratio = ratio;
    e.Setup( attackMs, releaseMs, sampleRate );
}

inline void CompressorNode::Setup( double threshold, double ratio, double attackMs, double releaseMs, int sampleRate )
{
    m_threshold = threshold;
    m_ratio = ratio;
    e.Setup( attackMs, releaseMs, sampleRate );
}
