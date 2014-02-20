//
//  AudioNodeFactory.h
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import <Foundation/Foundation.h>

@class AudioController;
@class AUAudioNode;

typedef enum {
    AUDIO_NODE_OUTPUT,
    AUDIO_NODE_MIXER,
    AUDIO_NODE_NETWORK,
    AUDIO_NODE_INVALID
} AUDIO_NODE_TYPE;

@interface AUAudioNodeFactory : NSObject {
    
}

// Kind of Hack-y but it works well
+(AudioController*) SetAudioController:(AudioController*)ac;
+(AudioController*) InitWithAudioController:(AudioController*)ac;

+(AUAudioNode*) MakeAudioNode:(AUDIO_NODE_TYPE)type;

@end
