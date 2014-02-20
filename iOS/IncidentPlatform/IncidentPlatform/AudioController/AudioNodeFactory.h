//
//  AudioNodeFactory.h
//  IncidentPlatform
//
//  Created by Idan Beck on 2/17/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AudioNode.h"

typedef enum {
    AUDIO_NODE_SAMPLE,
    AUDIO_NODE_INVALID
} AUDIO_NODE_TYPE;

@interface AudioNodeFactory : NSObject {
    
}

+(AudioNode*) MakeAudioNode:(AUDIO_NODE_TYPE)type;

@end
