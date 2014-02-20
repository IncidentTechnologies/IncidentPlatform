//
//  AudioNodeFactory.m
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import "AUAudioNodeFactory.h"

#import "AudioController.h"
#import "AUAudioNode.h"
#import "AUOutputNode.h"
#import "AUMixerNode.h"
#import "AUNodeNetwork.h"

@implementation AUAudioNodeFactory

+(AudioController*) SetAudioController:(AudioController*)ac {
    static AudioController *audioController = NULL;
    
    if(ac != NULL)
        audioController = ac;
    
    return audioController;
}

+(AudioController*) InitWithAudioController:(AudioController*)ac {
    return [AUAudioNodeFactory SetAudioController:ac];
}

+(AUAudioNode*)MakeAudioNode:(AUDIO_NODE_TYPE)type {
    AUAudioNode *audioNode = NULL;
    
    AudioController *ac = [AUAudioNodeFactory SetAudioController:NULL];
    if(ac == NULL)
        return NULL;
    
    switch(type) {
        case AUDIO_NODE_OUTPUT: {
            audioNode = (AUAudioNode*)[[AUOutputNode alloc] initWithAudioController:ac];
        } break;
            
        case AUDIO_NODE_MIXER: {
            audioNode = (AUAudioNode*)[[AUMixerNode alloc] initWithAudioController:ac];
        } break;
            
        case AUDIO_NODE_NETWORK: {
            audioNode = (AUAudioNode*)[[AUNodeNetwork alloc] initWithAudioController:ac];
        } break;
            
        case AUDIO_NODE_INVALID: {
            audioNode = NULL;
        } break;
    }
    
    return audioNode;
}

@end
