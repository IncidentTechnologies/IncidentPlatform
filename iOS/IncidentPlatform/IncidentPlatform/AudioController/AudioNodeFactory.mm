//
//  AudioNodeFactory.m
//  IncidentPlatform
//
//  Created by Idan Beck on 2/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "AudioNodeFactory.h"
#import "SampleNode.h"

@implementation AudioNodeFactory

+(AudioNode*)MakeAudioNode:(AUDIO_NODE_TYPE)type {
    RESULT r = R_SUCCESS;
    AudioNode *audioNode = NULL;
    
    switch(type) {
        case AUDIO_NODE_SAMPLE: {
            audioNode = (SampleNode*)new SampleNode((char *)[[[NSBundle mainBundle] pathForResource:@"TestGuitarSample" ofType:@"mp3"] UTF8String]);
        } break;
            
        case AUDIO_NODE_INVALID: {
            audioNode = NULL;
        } break;
    }
    
Error:
    return audioNode;
}

@end
