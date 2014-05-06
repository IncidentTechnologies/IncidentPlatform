//
//  EnvelopeFollowerNode.cpp
//  IncidentPlatform
//
//  Created by Kate Schnippering on 4/11/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#include "EnvelopeFollowerNode.h"


EnvelopeFollowerNode::EnvelopeFollowerNode()
{
    envelope=0;
}

void EnvelopeFollowerNode::Setup( double attackMs, double releaseMs, int sampleRate )
{
    attack = pow( 0.01, 1.0 / ( attackMs * sampleRate * 0.001 ) );
    release = pow( 0.01, 1.0 / ( releaseMs * sampleRate * 0.001 ) );
}

