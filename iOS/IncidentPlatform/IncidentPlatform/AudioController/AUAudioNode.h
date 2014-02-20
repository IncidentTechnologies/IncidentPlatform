//
//  AudioNode.h
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>

@class AudioController;
class CAStreamBasicDescription;

@interface AUAudioNode : NSObject {
    AUNode m_node;
    AudioComponentDescription m_description;
    
    AUGraph *m_pAUGraph;
    float m_GraphSampleRate;
    CAStreamBasicDescription *m_pStreamDescription;
}

- (id) initWithAudioController:(AudioController*)ac;
- (AUNode*) GetNode;
- (OSStatus) ConnectOutput:(int)outChannel toInput:(AUAudioNode*)inNode channel:(int)inChannel;

@end
