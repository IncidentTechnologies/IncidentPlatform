//
//  FileoutNode.h
//  IncidentPlatform
//
//  Created by Idan Beck on 3/30/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#ifndef __IncidentPlatform__FileoutNode__
#define __IncidentPlatform__FileoutNode__

#include <iostream>
#include "AudioNode.h"
#include <AudioToolbox/AudioToolbox.h>

class FileoutNode : public AudioNode {
public:
    FileoutNode(char *pszFilepath, bool fOverwrite);
    ~FileoutNode();
    
    AudioStreamBasicDescription GetClientFormatDescription(bool fStereo);
    
public:
    RESULT SaveSamples(unsigned long samples_n);
    
private:
    ExtAudioFileRef m_refFileout;
    unsigned long m_timestamp;
    
};

#endif /* defined(__IncidentPlatform__FileoutNode__) */
