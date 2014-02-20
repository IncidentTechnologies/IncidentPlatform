//
//  AudioNode.m
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import "AudioController.h"
#import "AUAudioNode.h"

#import "CAStreamBasicDescription.h"
#import "CAXException.h"

@implementation AUAudioNode

- (id) initWithAudioController:(AudioController*)ac {
    self = [super init];
    
    if(self) {
        m_pStreamDescription = new CAStreamBasicDescription();
        
        m_GraphSampleRate = AUDIO_CONTROLLER_SAMPLE_RATE;
        m_pAUGraph = [ac getAUGraph];  // AUGraph is static, so memory location should remail same
        memcpy(m_pStreamDescription, [ac GetDefaultStreamDescription], sizeof(CAStreamBasicDescription));
    }
    
    return self;
}

- (AUNode*) GetNode {
    return (&m_node);
}

- (OSStatus) ConnectOutput:(int)outChannel toInput:(AUAudioNode*)inNode channel:(int)inChannel {
    OSStatus result = AUGraphConnectNodeInput(*(m_pAUGraph), m_node, (UInt32)outChannel, *[inNode GetNode], (UInt32)inChannel);
    NSLog(@"AUAudioNode: Connect %d to %d: %s", outChannel, inChannel, CAX4CCString(result).get());
    return result;
}

@end
