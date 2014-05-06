//
//  AUNodeNetwork.h
//  IncidentPlatform
//
//  Created by Idan Beck on 2/13/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

// The node network encapsulates and allows the manipulation of audio nodes
// within a connected network for the creation of effects, synths, or anything else.
// This could eventually be exported out as an AU in future versions

#import "AUAudioNode.h"

@class AudioController;
class AudioNode;

@interface AUNodeNetwork : AUAudioNode {
    unsigned long int m_timestamp;
    AudioNode *m_rootNode;
    float m_volume;
    
    dispatch_semaphore_t m_sem;
}

- (unsigned long int) resetTimestamp;
- (unsigned long int) incrementTimestamp;

- (float) GetNextSample;
- (id) initWithAudioController:(AudioController*)ac;
- (OSStatus) Initialize;

- (AudioNode*) GetRootNode;
- (OSStatus) SetVolume:(float)volume;

- (int) WaitOnSemaphore;
- (int) ReleaseSemaphore;

@end
