//
//  OutputNode.m
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import "AUOutputNode.h"
#import "AudioController.h"
#import "CAXException.h"

@implementation AUOutputNode

- (id) initWithAudioController:(AudioController*)ac {
    self = [super initWithAudioController:ac];

    if(self) {
        // Output Component
        m_description.componentType = kAudioUnitType_Output;
        m_description.componentSubType = kAudioUnitSubType_RemoteIO;
        m_description.componentFlags = 0;
        m_description.componentFlagsMask = 0;
        m_description.componentManufacturer = kAudioUnitManufacturer_Apple;
        
        OSStatus status = AUGraphAddNode(*(m_pAUGraph), &m_description, &m_node);
        NSLog(@"Add AUOutput Node: %s", CAX4CCString(status).get());
    }
    
    return self;
}

@end
