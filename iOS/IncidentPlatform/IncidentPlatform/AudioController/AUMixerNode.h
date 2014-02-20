//
//  MixerNode.h
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import "AUAudioNode.h"

@class AudioController;

@interface AUMixerNode : AUAudioNode {
    int m_busses_n;
}

-(int) GetBusCount;
-(id) initWithAudioController:(AudioController*)ac;
-(OSStatus) SetBusCount:(int)bus_n;
-(OSStatus) InitializeMixerBusses;

@end
