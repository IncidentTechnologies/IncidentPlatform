//
//  OutputNode.h
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import "AUAudioNode.h"

@class AudioController;

@interface AUOutputNode : AUAudioNode {
    
}

- (id) initWithAudioController:(AudioController*)ac;
- (OSStatus) SetVolume:(float)volume;

@end
