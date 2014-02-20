//
//  AudioNodeNetwork.h
//  IncidentPlatform
//
//  Created by Idan Beck on 2/18/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__AudioNodeNetwork__
#define __IncidentPlatform__AudioNodeNetwork__

#include <iostream>
#include "AudioNode.h"

// An audio node network allows you to create discrete
// networks of nodes that act as one.  This would be how
// a sampler object would be constructed for example

class AudioNodeNetwork : public AudioNode {
public:
    AudioNodeNetwork();
    ~AudioNodeNetwork();
    
    float GetNextSample(unsigned long int timestamp);
    
protected:
    AudioNode *m_inputNode;
    AudioNode *m_outputNode;
    
    AudioNode *m_cursorNode;
};

#endif /* defined(__IncidentPlatform__AudioNodeNetwork__) */
