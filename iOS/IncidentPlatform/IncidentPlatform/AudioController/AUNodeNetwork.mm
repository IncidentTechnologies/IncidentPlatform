//
//  AUNodeNetwork.m
//  IncidentPlatform
//
//  Created by Idan Beck on 2/13/14.
//  Copyright (c) 2014 Incident Technologies, Inc. All rights reserved.
//

#import "AUNodeNetwork.h"

#import "AudioNode.h"
#import "AudioController.h"

#import "CAStreamBasicDescription.h"
#import "CAXException.h"

#import <semaphore.h>


@implementation AUNodeNetwork

- (id) initWithAudioController:(AudioController*)ac {
    self = [super initWithAudioController:ac];
    
    if(self) {
        // Set up the root node
        m_rootNode = new AudioNode();
        m_rootNode->SetChannelCount(1, CONN_IN);
        m_timestamp = 0;
        
        // Output Component
        m_description.componentType = kAudioUnitType_Output;
        m_description.componentSubType = kAudioUnitSubType_RemoteIO;
        m_description.componentFlags = 0;
        m_description.componentFlagsMask = 0;
        m_description.componentManufacturer = kAudioUnitManufacturer_Apple;
        
        OSStatus status = AUGraphAddNode(*(m_pAUGraph), &m_description, &m_node);
        NSLog(@"Add AUOutput Node: %s", CAX4CCString(status).get());
        
        m_volume = 1.0f;
        
        // Create semaphore with 1 count
        _m_sem = dispatch_semaphore_create(1);
        
        
    }
    
    return self;
}

- (unsigned long int) resetTimestamp { return (m_timestamp = 0); }
- (unsigned long int) incrementTimestamp { return ++m_timestamp; }
- (AudioNode*) GetRootNode { return m_rootNode; }

- (float) GetNextSample {
    float retVal = m_rootNode->GetNextSample(m_timestamp);
    m_timestamp++;
    return retVal;
}

- (int) WaitOnSemaphore {
    //NSLog(@"Wait on Semaphore");

    return dispatch_semaphore_wait(_m_sem, DISPATCH_TIME_FOREVER);
}

- (int) ReleaseSemaphore {
    //NSLog(@"Release Semaphore");

    return dispatch_semaphore_signal(_m_sem);
}

- (dispatch_semaphore_t)TakeSemaphore {
    return _m_sem;
}

// Audio Render Callback Procedure
static OSStatus renderNetwork(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp,
                               UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
    
    AUNodeNetwork *net = (__bridge AUNodeNetwork*)(inRefCon);
    
    dispatch_semaphore_wait([net TakeSemaphore], DISPATCH_TIME_FOREVER);

    AudioSampleType *outA = (AudioSampleType*)ioData->mBuffers[0].mData;
    
    for(UInt32 i = 0; i < inNumberFrames; i++) {
        SInt16 nextSample = (SInt16)([net GetNextSample] * 32767.0f * net->m_volume);
        outA[i] = nextSample;
    }
    
    dispatch_semaphore_signal([net TakeSemaphore]);
    
	return noErr;
}

-(OSStatus) Initialize {
    OSStatus status;
    
    AudioUnit generatorAudioUnit;
    status = AUGraphNodeInfo(*(m_pAUGraph), m_node, NULL, &generatorAudioUnit);
    NSLog(@"GetGeneratorAudioUnit: %s", CAX4CCString(status).get());
    
    // Set up the render callback struct
    AURenderCallbackStruct renderCallbackStruct;
    renderCallbackStruct.inputProc = &renderNetwork;
    renderCallbackStruct.inputProcRefCon = (void*)CFBridgingRetain(self);
    
    // Set a callback for the specified node's specified output
    status = AUGraphSetNodeInputCallback(*(m_pAUGraph), m_node, 0, &renderCallbackStruct);
    NSLog(@"Generator: Set Input Callback chn: %d: %s", 0, CAX4CCString(status).get());
    
    printf("Mixer File Format:");
    m_pStreamDescription->Print();
    
    // Apply the modified CAStreamBasicDescription to the mixer output bus
    status = AudioUnitSetProperty(generatorAudioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Input,
                                  0,
                                  m_pStreamDescription,
                                  sizeof(CAStreamBasicDescription));
    
    NSLog(@"Generator: Set Stream Intput Format chn: %d: %s", 0, CAX4CCString(status).get());
    
    // Apply the modified CAStreamBasicDescription to the mixer output bus
    status = AudioUnitSetProperty(generatorAudioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output,
                                  0,
                                  m_pStreamDescription,
                                  sizeof(CAStreamBasicDescription));
    
    
    NSLog(@"Generator: Set Stream Output Format chn: %d: %s", 0, CAX4CCString(status).get());
    
    
    return status;
}

- (OSStatus) SetVolume:(float)volume {
    OSStatus status = 0;
    
    //AudioUnit _node;
    //status = AUGraphNodeInfo(*(m_pAUGraph), m_node, NULL, &_node);
    //status = AudioUnitSetParameter(_node, kHALOutputParam_Volume, kAudioUnitScope_Output, 0, volume, 0);
    
    m_volume = volume;
    return status;
}

@end
