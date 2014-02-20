//
//  MixerNode.m
//  AudioController
//
//  Created by Idan Beck on 2/11/14.
//
//

#import "AUMixerNode.h"
#import "CAStreamBasicDescription.h"
#import "CAXException.h"

@implementation AUMixerNode

- (id) initWithAudioController:(AudioController*)ac {
    self = [super initWithAudioController:ac];
    
    if(self) {
        // Output Component
        m_description.componentType = kAudioUnitType_Mixer;
        m_description.componentSubType = kAudioUnitSubType_MultiChannelMixer;
        m_description.componentFlags = 0;
        m_description.componentFlagsMask = 0;
        m_description.componentManufacturer = kAudioUnitManufacturer_Apple;
        
        OSStatus status = AUGraphAddNode(*(m_pAUGraph), &m_description, &m_node);
        NSLog(@"Add AUMixer Node: %s", CAX4CCString(status).get());
    }
    
    return self;
}

-(int) GetBusCount {
    return m_busses_n;
}

-(OSStatus) SetBusCount:(int)bus_n {
    m_busses_n = bus_n;
    UInt32 busCount = (UInt32)m_busses_n;
    
    AudioUnit mixerAudioUnit;
    OSStatus status = AUGraphNodeInfo(*(m_pAUGraph), m_node, NULL, &mixerAudioUnit);
    
    status = AudioUnitSetProperty(mixerAudioUnit,
                                  kAudioUnitProperty_ElementCount,
                                  kAudioUnitScope_Input,
                                  0,
                                  &busCount,
                                  sizeof(busCount));
    
    NSLog(@"Set Bus Count: %s", CAX4CCString(status).get()); 
    
    return status;
}

/*
// Audio Render Callback Procedure
// Don't allocate memory, don't take any locks, don't waste time
// TODO: Should put into the pertinent places
static OSStatus renderInput(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp,
							UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
	// Get a reference to the object that was passed with the callback
	//AudioController *ac = (AudioController*)inRefCon;
	
	// Get a pointer to the dataBuffer of the AudioBufferList
	AudioSampleType *outA = (AudioSampleType*)ioData->mBuffers[0].mData;
	
    static float phase = 0.0;
	
	// Loop through the callback buffer, generating the samples
	for(UInt32 i = 0; i < inNumberFrames; i++) {
		float tempSample = sin(phase);
        phase += ((2.0 * M_PI) / 44100.0) * 440.0;
        
        outA[i] = (SInt16)(tempSample * 32767.0f);
        
        //Limit at 32700
        if(outA[i] > 32700)
            outA[i] = 32700;
        else if(outA[i] < -32700)
            outA[i] = -32700;
    }
    
    NSLog(@"hi");
	
	return noErr;
}
*/

-(OSStatus) InitializeMixerBusses {
    OSStatus status;
    
    AudioUnit mixerAudioUnit;
    status = AUGraphNodeInfo(*(m_pAUGraph), m_node, NULL, &mixerAudioUnit);
    NSLog(@"GetMixerAudioUnit: %s", CAX4CCString(status).get());
	
	// Loop through and set up a callback for each source sent to the mixer
	// We are only doing a single bus so the loop is a little redundant
	for(int i = 0; i < m_busses_n; ++i) {
		/*
        // Set up the render callback struct
		AURenderCallbackStruct renderCallbackStruct;
		renderCallbackStruct.inputProc = &renderInput;
		renderCallbackStruct.inputProcRefCon = self;
		
		// Set a callback for the specified node's specified output
		status = AUGraphSetNodeInputCallback(*(m_pAUGraph), m_node, i, &renderCallbackStruct);
         */
		
		printf("Mixer File Format:");
		m_pStreamDescription->Print();
		
		// Apply the modified CAStreamBasicDescription to the mixer output bus
		status = AudioUnitSetProperty(mixerAudioUnit,
									  kAudioUnitProperty_StreamFormat,
									  kAudioUnitScope_Input,
									  i,
									  m_pStreamDescription,
									  sizeof(CAStreamBasicDescription));
        
        NSLog(@"Set Stream Format chn: %d: %s", i, CAX4CCString(status).get());
	}
    
	// Apply the modified CAStreamBasicDescription to the mixer output bus
	status = AudioUnitSetProperty(mixerAudioUnit,
								  kAudioUnitProperty_StreamFormat,
								  kAudioUnitScope_Output,
								  0,
								  m_pStreamDescription,
								  sizeof(CAStreamBasicDescription));
    
    NSLog(@"Set Stream Format chn: %d: %s", 0, CAX4CCString(status).get());

    return status;
}

@end
